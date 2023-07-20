#ifndef SNMP_H
#define SNMP_H

#include <cstring>
#include <algorithm>
#include <vector>
#include <map>

#define CHILD_COUNT 128

//typedef struct PAYLOAD {

//    void* payload = 0;
//    unsigned int payload_int = 0;

//} BER_PAYLOAD;

//typedef struct ITEM {

//    unsigned char type = 0;        // 0 - NODE, 1 - payload
//    BER_PAYLOAD* payload = 0;
//    void* node = 0;

//    void set_node(void* node)
//    {
//        this->node = node;
//        type = 0;
//    }

//    void set_pyload(BER_PAYLOAD* payload)
//    {
//        this->payload;
//        type = 1;
//    }

//} DATA_ITEM;

typedef struct NODE {

    char identifier = 0;
    int idx = -1;
    unsigned int data_len = 0;                      // length of sub-tree including this node without identifier and lengh part
    void* payload = 0;
    unsigned int payload_len = 0;                   // only payload length
    NODE* children[CHILD_COUNT] {};

    /**
     * \brief Initializes stucture like constructor
     */
    void init()
    {
        identifier = 0;
        idx = -1;
        data_len = 0;                               // length of sub-tree including this node without identifier and lengh part
        payload = 0;
        payload_len = 0;                            // only payload length
        std::memset(children, 0, CHILD_COUNT * sizeof (NODE*));
    }

    /**
     * \brief Set payload
     * \param payload
     */
    void set_payload(void* payload, unsigned int size)
    {
       payload_len = size;
       if ( !(this->payload = std::malloc(payload_len)) )
       {
           printf("Payload allocation error. Low memory.");
           exit(1);
       }

       std::memcpy(this->payload, payload, payload_len);
    }

    /**
     * \brief Free sub-tree memory
     */
    void free_mem()
    {
        for (NODE* child : children)
        {
            if ( !child ) break;
            child->free_mem();
            std::free(child);
        }

        free(payload);
    }

    /**
     * \brief Add child to node. Max count is 128.
     * \param child node
     */
    bool add_child(NODE* child)
    {
        if ( idx == CHILD_COUNT-1 )
            return false;
        children[++idx] = child;

        return true;
    }

    /**
     * \brief Calculate count of feed bytes in var
     * \param var Variable
     * \return count of feed bytes
     */
    unsigned char feed_bytes(uint64_t var)
    {
        if ( var == 0 )
            return 0;
        else
        {
            size_t len = sizeof (var);
            for ( char i=len-1; i>=0; i-- )
            {
                unsigned char val = (unsigned char) (var >> i * 8);
                if ( val != 0)
                    return ++i;
            }

            return 0;
        }
    }

    /**
     * \brief Return length of BER length part
     * \param len Length of payload
     * \return length of length part by BER
     */
    unsigned char get_ber_part_len(unsigned int len)
    {
        /**
          Возвращает количество байтов, которое занимает секция, описывающая длину закодированных данных (длину секции длины)
         **/
        if ( len <= 127 )
            return 1;
        else
            return feed_bytes(len) + 1;
    }

    /**
     *  \brief Get length of complete item (with identifier, length and value) as bytes by BER
     *  \param Payload length
     */
    unsigned int get_item_len()
    {
        unsigned int children_len = 0;
        for ( NODE* child : children )
        {
            if ( !child ) break;
            children_len += child->get_item_len();
        }

        data_len = children_len + payload_len;
        return sizeof (identifier) + get_ber_part_len(data_len) + data_len;
    }

    /**
     * \brief encode array of unsigned int numbers to appropriate array of bytes by BER.
     * \param nums array of ints
     * \param size count of elements in array
     */
    void set_num_payload(uint32_t* nums, int size)
    {
        std::vector<char> res (0);
        for ( int idx=0; idx<size; idx++ )
        {
            if ( nums[idx] <= 127 )
                res.push_back((char) nums[idx]);
            else
            {
                std::vector<char> v = encode_num(nums[idx]);
                res.insert(res.end(), v.begin(), v.end());
            }
        }


        // set payload
        this->set_payload(&res[0], res.size());
    }

    /**
     * \brief encode number by BER. If number is biger than 127 it encodes it as more than one bytes
     * \param num
     * \return vector of bytes
     */
    std::vector<char> encode_num(uint32_t num)
    {
        /**
         Целое число кодируется по следующему правилу:
           - Если кодируемое чило меньше либо равно 127, то старший бит устанавливается в 0, а кодируемое число записывается в оставшиеся семь бит
           - Если число больше 127, то старший бит всех байт кроме последнего устанавливается в 1. Страший бит последнего устанавливается в 0.
             Само число последовательно записывается в младший семь бит каждого байта. Например, 100101 01110001B = 0x25 0x71 = 9585 в закодированном виде
             будет выглядеть так: 11001010B 01110001B = 0xCA 0x71
         **/
        std::vector<char> v (0);
        if ( num <= 127 )
        {
            v.push_back(num);
            return v;
        }
        // template to take 7 lower bites
        uint32_t bitwise_temp = 127;

        for (unsigned int idx=0; idx <= sizeof (num)*8/7; idx++ )
        {
            char item = ( (num & bitwise_temp) >> idx * 7 );
            // first 7 bites are always valid
            if ( idx == 0 )
            {
                v.push_back(item);
                bitwise_temp = bitwise_temp << 7;
                continue;
            }

            if ( item == 0 )
                break;
            else
            {
                v.push_back(item);
                bitwise_temp = bitwise_temp << 7;
            }
        }

        std::reverse(std::begin(v), std::end(v));
        v[0] |= 128;

        return v;
    }

    /**
     * \brief Get array that describes length of payload by BER
     * \param len Length of payload
     * \param buf buffer to write into
     * \param start point to write from
     * \return ptr to array
     */
    void get_len_part(unsigned int len, char* buf, int* start)
    {
        /** Если длина не превышает 127 октетов (байт), то она просто записывается в октет длины.
        Если длина блока закодированных данных больше 127 байт, то:
            - в биты второго и последующих октетов записывается значение длины блока закодированной информации в её (длины) двоичном представлении;
            - в первый октет записывается количество дополнительных блоков длины, считая со второго; бит 8 первого октета устанавливается в 1.
        **/
        unsigned char part_len = get_ber_part_len(len);
        if ( part_len == 1 )
        {
            (*start)++;
            buf[*start] = (char) len;
        }
        else
        {
            unsigned char bytes = part_len-1;                // Count of feed bytes
            (*start)++;
            buf[*start] = (unsigned char) 128 | bytes;
            for ( int i=0; i<bytes; i++ ){
                (*start)++;
                buf[*start] = (unsigned char) (len >> i * 8);
            }
        }
    }

    /**
     * \brief Get sub-tree frame
     * \param buf Buffer to fill
     * \param start Start index to fill
     */
    void get_frame(char* buf, int* start)
    {
        // fill id
        buf[*start] = identifier;
        // fill len
        get_len_part(data_len, buf, start);

        // fill children
        for ( NODE* child : children )
        {
            if ( !child ) break;
            (*start)++;
            child->get_frame(buf, start);
        }

        // fill payload
        if ( payload )
        {
            (*start)++;
            std::memcpy(buf + *start, payload, payload_len);
            *start += payload_len - 1;
        }
    }

} BER_NODE;


typedef struct {

    BER_NODE root;
    BER_NODE* oids_basket;

    char* frame = 0;
    uint64_t frame_len = 0;

    BER_NODE* get_node()
    {
        BER_NODE* new_node;
        if ( !(new_node = (BER_NODE*) std::malloc(sizeof (BER_NODE))) )
        {
            printf("Node allocation error. Low memory.");
            exit(1);
        }

        new_node->init();
        return new_node;
    }

    /**
     * \brief Crete SNMP message
     * \param ver Version of snmp
     * \param pass Community string
     */
    void init_message(unsigned char ver, char* pass, unsigned int pass_len)
    {
        root.identifier = 0x30;

        // version
        BER_NODE* vers = get_node();
        vers->identifier = 0x2;
        vers->set_payload(&ver, sizeof (ver));
        root.add_child(vers);

        // community string
        BER_NODE* passwd = get_node();
        passwd->identifier = 0x4;
        passwd->set_payload(pass, pass_len);
        root.add_child(passwd);

        // Nodes for next request

        // id
        BER_NODE* id = get_node();
        id->identifier = 0x2;
        char id_v = 0x1;
        id->set_payload(&id_v, sizeof (id_v));

        // error status
        BER_NODE* err_stat = get_node();
        err_stat->identifier = 0x2;
        char err_stat_v = 0x0;
        err_stat->set_payload(&err_stat_v, sizeof (err_stat_v));

        // error index
        BER_NODE* err_idx = get_node();
        err_idx->identifier = 0x2;
        char err_idx_v = 0x0;
        err_idx->set_payload(&err_idx_v, sizeof (err_idx_v));

        // oids container
        oids_basket = get_node();
        oids_basket->identifier = 0x30;

        // next request
        BER_NODE* next_req = get_node();
        next_req->identifier = 0xa0;
        next_req->add_child(id);
        next_req->add_child(err_stat);
        next_req->add_child(err_idx);
        next_req->add_child(oids_basket);

        root.add_child(next_req);
    }

    /**
     * \brief Add oid to message
     * \param oid uint32 array
     * \param oid_size count of elements in array
     */
    void add_oid(uint32_t* oid, unsigned int oid_size)
    {
        BER_NODE* oid_node = get_node();
        oid_node->identifier = 0x6;
        oid_node->set_num_payload(oid, oid_size);

        BER_NODE* oid_holder = get_node();
        oid_holder->identifier = 0x30;
        oid_holder->add_child(oid_node);
        char end[2] = {0x5, 0x0} ;
        oid_holder->set_payload(end, sizeof (end));

        oids_basket->add_child(oid_holder);
    }

    /**
     * \brief Create SNMP frame for transmition
     */
    char* commit()
    {
        frame_len = get_total_len();
        frame = (char*) std::malloc(frame_len);
        int idx = 0;
        root.get_frame(frame, &idx);
        return frame;
    }

    /**
     * \brief Calculate total length of message
     * \return
     */
    uint32_t get_total_len()
    {
        return root.get_item_len();
    }

    /**
     * \brief Free all resources
     */
    void free()
    {
        root.free_mem();
//        std::free(frame);
    }

} SNMP_MESSAGE;

typedef struct {

    uint32_t val1;
    uint32_t val2;
    uint32_t val3;
    uint32_t val4;
    uint32_t val5;

} UPS_RESPONSE;

uint32_t get_int_from_bytes(char* bytes, uint32_t* idx, uint32_t count);

uint32_t get_len(char* package, uint32_t* idx);

uint32_t get_int(char* package, uint32_t* idx);

std::string get_string(char* package, uint32_t* idx);

void push_oid_to_vec(std::vector<uint32_t>* vec, uint32_t oid);

std::vector<uint32_t> decode_oid_from_bytes(char* bytes, uint32_t* idx, uint32_t count);

void parse_oid_holder(char* package, uint32_t* idx, UPS_RESPONSE* values);

UPS_RESPONSE* parse_snmp(char* package);

#endif // SNMP_H
