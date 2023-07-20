#include <vector>
#include <map>
#include <exception>
#include <snmp.h>

/**
 * \brief Concatenates values of some bytes to form unsigned integer. //Объединяет значения некоторых байтов в целое число без знака.
 * \param bytes where value of integer is located//байты, в которых находится значение целого числа
 * \param idx index of first byte// индекс первого байта
 * \param count count of bytes to read// количество байтов для чтения
 * \return unsigned integer
 */
uint32_t get_int_from_bytes(char* bytes, uint32_t* idx, uint32_t count)
{
    if ( count > 4 ) throw std::exception();
    uint32_t res = 0;
    uint32_t end_len_idx = *idx + count;
    while ( *idx < end_len_idx )
    {
        res = res << 8;
        res |= (unsigned char) bytes[*idx];
        (*idx)++;
    }
    (*idx)--;

    return res;
}

/**
 * \brief Parse the length of snmp structure.
 * \param package snmp package
 * \param idx index from which parse
 * \return length of snmp stucture
 */
uint32_t get_len(char* package, uint32_t* idx)
{

    if ( (unsigned char) package[*idx] <= 127 ) return package[*idx];
    else
    {
        char bytes_count = 127 & package[*idx];
        (*idx)++;
        return get_int_from_bytes(package, idx, bytes_count);
    }
}

/**
 * \brief Parse integer snmp structure.
 * \param package snmp package
 * \param idx index from which parse
 * \return integer value
 */
uint32_t get_int(char* package, uint32_t* idx)
{
    if ( package[*idx] != 0x02 ) throw std::exception();
    (*idx)++;
    uint32_t len = get_len(package, idx);
    (*idx)++;
    return get_int_from_bytes(package, idx, len);
}

/**
 * \brief Parse integer snmp structure.
 * \param package snmp package
 * \param idx index from which parse
 * \return string value
 */
std::string get_string(char* package, uint32_t* idx)
{
    if ( package[*idx] != 0x04 ) throw std:: exception();
    (*idx)++;
    uint32_t len = get_len(package, idx);
    std::string res = std::string(package[*idx+1], len);
    *idx += len;
    return res;
}

/**
 * \brief push oid to vector. It correctly interprets first value.
 * \param vec
 * \param num
 */
void push_oid_to_vec(std::vector<uint32_t>* vec, uint32_t oid)
{
    if ( vec->empty() && oid > 40 )
    {
        vec->push_back(1);
        vec->push_back(oid - 40);
    }
    else
        vec->push_back(oid);
}

/**
 * \brief Decode integer values of oids which are encoded as set of bytes. There may be one or more oids.
 * \param bytes bytes where oids are encoded
 * \param idx index of first byte
 * \param count count bytes to decode
 * \return vector of oids
 */
std::vector<uint32_t> decode_oid_from_bytes(char* bytes, uint32_t* idx, uint32_t count)
{
    uint32_t decoded_num = 0;
    std::vector<uint32_t> res (0);
    uint32_t mask = 127;
    uint32_t decoded_byte_idx = 0;
    uint32_t end_idx = *idx + count;
    while ( *idx < end_idx )
    {
        if ( (unsigned char) bytes[*idx] <= 127 )
        {
            // push previously decoded num
            if ( decoded_num != 0 )
            {
                push_oid_to_vec(&res, decoded_num);
                decoded_num = 0;
            }
            push_oid_to_vec(&res, bytes[*idx]);
            (*idx)++;
        }
        else
        {
            decoded_num = (decoded_num << decoded_byte_idx * 7) | ((unsigned char) (bytes[*idx]) & mask);
            decoded_byte_idx++;
            (*idx)++;
        }
    }
    (*idx)--;

    if ( decoded_num != 0 ) push_oid_to_vec(&res, decoded_num);

    return res;
}

/**
 * \brief Parse the structure that contains oids and their values.
 * \param package snmp package
 * \param idx index of first byte
 * \param values map to store oids and their values
 */
void parse_oid_holder(char* package, uint32_t* idx, UPS_RESPONSE* values)
{
    if ( package[*idx] != 0x30 ) return;

    (*idx)++;
    get_len(package, idx);
    (*idx)++;

    // Object Identifier
    if ( package[*idx] != 0x06 ) return;
    (*idx)++;
    uint32_t len = get_len(package, idx);
    (*idx)++;

    // get oid as set of ints
    std::vector<uint32_t> ids = decode_oid_from_bytes(package, idx, len);
    // create oid name as string
    std::string oid_name = "";
    int size = ids.size();
    for ( int i = 0; i < size; i++ )
    {
        oid_name += std::to_string(ids[i]);
        if ( i < size - 1 ) oid_name += '.';
    }

    (*idx)++;
//    (*values)[oid_name] = get_int(package, idx);
    values->val1 = get_int(package, idx);
}

/**
 * \brief Parse snmp package
 * \param package snmp package as array of bytes
 * \return map of oids and their values
 */
UPS_RESPONSE* parse_snmp(char* package)
{
    // index of
    uint32_t idx = 0;
    // main structure
    if ( package[idx] != 0x30 )
    {
        printf("\nNot 0x30 of main structure\n");
        return {};
    }

    // length of main structure
    uint32_t main_len = get_len(package, &(++idx));
    uint32_t last_packge_idx = main_len + idx;

    // version
    uint32_t ver = get_int(package, &(++idx));
    if ( ver != 0 )
    {
        printf("\nWrong community version: %d%s", ver, "\n");
        return {};
    }

    // community string
    if ( !get_string(package, &(++idx)).compare("public") )
    {
        printf("\nWrong community string\n");
        return {};
    }

    // get response identifier
    if ( (unsigned char) package[++idx] != 0xa2 )
    {
        printf("\nWrong response id\n");
        return {};
    }

    // get response length
    get_len(package, &(++idx));

    // id
    uint32_t id = get_int(package, &(++idx));
    if ( id != 1 )
    {
        printf("\nWrong id\n");
        return {};
    }

    // error status
    uint32_t err_stat = get_int(package, &(++idx));
    if ( err_stat != 0 )
    {
        printf("\nerror status: %d%s", err_stat, "\n");
        return {};
    }

    // error index
    uint32_t err_idx = get_int(package, &(++idx));
    if ( err_idx != 0 )
    {
        printf("\nerror index: %d%s", err_idx, "\n");
        return {};
    }

    // oids basket
    if ( package[++idx] != 0x30 )
    {
        printf("\nNot 0x30 of oids basket\n");
        return {};
    }

    uint32_t basket_len = get_len(package, &(++idx));
    uint32_t basket_end_idx = idx+basket_len;

    UPS_RESPONSE* response = (UPS_RESPONSE*) std::malloc(sizeof (UPS_RESPONSE));
    while ( idx <  basket_end_idx)
    {
        parse_oid_holder(package, &(++idx), response );
    }

    if ( idx > last_packge_idx )
    {
        printf("\nWrong end index of package: %d%s", idx, "\n");
        return {};
    }

    return response;
}
