#include "unity.h"
#include "phev_core.h"
#include "msg_utils.h"

const uint8_t singleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x00, 0x06, 0x06, 0x13, 0x05, 0x13, 0x01, 0xc3};
const uint8_t doubleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x00, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff, 0x6f, 0x0a, 0x00, 0x13, 0x00, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff};

void test_create_phev_message(void)
{
    uint8_t data[] = {0,1,2,3,4,5};
    phevMessage_t * message = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(0x6f, message->command);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, message->type);
    TEST_ASSERT_EQUAL(0x12, message->reg);
    TEST_ASSERT_EQUAL(sizeof(data), message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, message->data, sizeof(data));
} 

void test_destroy_phev_message(void)
{
    uint8_t data[] = {0,1,2,3,4,5};
    phevMessage_t * message = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    TEST_ASSERT_NOT_NULL(message);
    phev_core_destroyMessage(message);
    
} 

void test_split_message_single_correct_size(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(1, ret);
}

void test_split_message_single_correct_command(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(0x6f, msg.command);
} 
void test_split_message_single_correct_length(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(7, msg.length);
} 
void test_split_message_single_correct_type(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg.type);
} 
void test_split_message_single_correct_reg(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
} 
void test_split_message_single_correct_data(void)
{
    phevMessage_t msg;
    uint8_t data[] = {0x00, 0x06, 0x06, 0x13, 0x05, 0x13};

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, msg.data, 6);
} 
void test_split_message_double_correct(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(doubleMessage, sizeof(doubleMessage), &msg);

    ret = phev_core_decodeMessage(doubleMessage + ret, sizeof(singleMessage) - ret, &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
} 
void test_split_message_double_decode(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(doubleMessage, sizeof(doubleMessage), &msg);

    ret = phev_core_decodeMessage(doubleMessage + ret, sizeof(doubleMessage) - ret, &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
} 
void test_encode_message_single(void)
{
    uint8_t data[] = {0x00, 0x06, 0x06, 0x13, 0x05, 0x13,0x01};
    phevMessage_t * msg = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    uint8_t * out;
    int num = phev_core_encodeMessage(msg, &out);
    TEST_ASSERT_EQUAL(12, num);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(singleMessage, out, num);
} 
void test_encode_message_single_checksum(void)
{
    uint8_t data[] = {0x00};

    phevMessage_t  * msg = phev_core_createMessage(0x6f, RESPONSE_TYPE, 0xaa, data, 1);
    
    uint8_t * out;
    int num = phev_core_encodeMessage(msg, &out);
    TEST_ASSERT_EQUAL(0x1e, out[5]);
} 
void test_simple_command_request_message(void)
{
    phevMessage_t *msg = phev_core_simpleRequestCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
} 
void test_simple_command_response_message(void)
{
    phevMessage_t *msg = phev_core_simpleResponseCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
} 
void test_command_message(void)
{
    uint8_t data[] = {0, 1, 2, 3, 4, 5};

    phevMessage_t *msg = phev_core_commandMessage(0x10, data, sizeof(data));

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x6, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data, msg->data, sizeof(data));
} 
void test_ack_message(void)
{
    const uint8_t reg = 0x10;
    const phevMessage_t *msg = phev_core_ackMessage(0x6f, reg);

    TEST_ASSERT_EQUAL(0x6f, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL(0x00, *msg->data);
} 
void test_start_message(void)
{
    uint8_t mac[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    const uint8_t expected[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,0x00};
    
    phevMessage_t *msg = phev_core_startMessage(mac);

    TEST_ASSERT_NOT_NULL(msg);
    TEST_ASSERT_EQUAL(START_SEND, msg->command);
    TEST_ASSERT_EQUAL(0x07, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x01, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, msg->data, sizeof(expected));
} 
void test_start_encoded_message(void)
{
    uint8_t mac[] = {0,0,0,0,0,0};
    uint8_t expected[] = {0xf2, 0x0a, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 
        0xf6, 0x04, 0x00, 0xaa, 0x00, 0xa4};

    message_t *message = phev_core_startMessageEncoded(mac);

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(18, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data, message->length);
} 
void test_ping_message(void)
{
    const uint8_t num = 1;

    phevMessage_t * msg = phev_core_pingMessage(num);

    TEST_ASSERT_EQUAL(PING_SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x01, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(num, msg->reg);
    TEST_ASSERT_EQUAL(0, msg->data[0]);
}
void test_response_handler_start(void)
{
    uint8_t value = 0;
    phevMessage_t request = {
        .command = RESP_CMD,
        .length = 4,
        .type = REQUEST_TYPE,
        .reg = 0x29,
        .data = &value,  
    };
    phevMessage_t * msg = phev_core_responseHandler(&request);

    TEST_ASSERT_EQUAL(SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x01, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x29, msg->reg);
    TEST_ASSERT_EQUAL(0, *msg->data);   
} 
void test_calc_checksum(void)
{
    const uint8_t data[] = {0x2f,0x04,0x00,0x01,0x01,0x00};
    uint8_t checksum = phev_core_checksum(data);
    TEST_ASSERT_EQUAL(0x35,checksum);
} 
void test_phev_message_to_message(void)
{
    phevMessage_t * phevMsg = phev_core_simpleRequestCommandMessage(0xaa, 0x00);
    int8_t expected[] = {0xf6,0x04,0x00,0xaa,0x00,0xa4};
    
    message_t * message = phev_core_convertToMessage(phevMsg);

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(6, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data,6); 
}

void test_phev_ack_message(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x01, 0xc0, 0x00, 0xbb};
    const phevMessage_t * phevMsg = phev_core_ackMessage(0xf6,0xc0);
    
    TEST_ASSERT_NOT_NULL(phevMsg);
    TEST_ASSERT_EQUAL(0xf6, phevMsg->command);
    TEST_ASSERT_EQUAL(0x01, phevMsg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, phevMsg->type);
    TEST_ASSERT_EQUAL(0, phevMsg->data[0]);
} 
void test_phev_head_lights_on(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x00, 0x0a, 0x01, 0x05};
    
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(0x0a, 1);
    message_t * message = phev_core_convertToMessage(headLightsOn);
    
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data,6); 
    
}
void test_phev_head_lights_on_message(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x00, 0x0a, 0x01, 0xbb};
    
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(0x0a, 1);
    
    TEST_ASSERT_EQUAL(1, headLightsOn->data[0]);
}
void test_phev_mac_response(void)
{
    uint8_t message[] = {0x2f,0x04,0x01,0x01,0x00,0x35};
    phevMessage_t phevMsg;

    phev_core_decodeMessage(message, sizeof(message), &phevMsg);

    TEST_ASSERT_EQUAL(START_RESP, phevMsg.command);
    TEST_ASSERT_EQUAL(1, phevMsg.length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, phevMsg.type);
    TEST_ASSERT_EQUAL(1, phevMsg.reg);
    TEST_ASSERT_EQUAL(0, *phevMsg.data);

}
void test_phev_message_to_phev_message_and_back(void)
{
     uint8_t message[] = {0x2f,0x04,0x01,0x01,0x00,0x35};

     phevMessage_t phevMsg;

     phev_core_decodeMessage(message, sizeof(message), &phevMsg);

     message_t * msg = phev_core_convertToMessage(&phevMsg);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(message, msg->data, sizeof(message));
    
}

void test_phev_core_copyMessage(void)
{
    uint8_t data[] = {0x01,0x04,0x01,0x01,0x00,0x35};

    phevMessage_t  * msg = phev_core_createMessage(0x6f, RESPONSE_TYPE, 0xaa, data, sizeof(data));
    
    phevMessage_t * copy = phev_core_copyMessage(msg);

    TEST_ASSERT_NOT_NULL(copy);
    TEST_ASSERT_EQUAL(0x6f,copy->command);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE,copy->type);
    TEST_ASSERT_EQUAL(0xaa,copy->reg);
    TEST_ASSERT_EQUAL(sizeof(data),copy->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, copy->data, sizeof(data));
    
}
void test_phev_core_my18_xor_decodeMessage_ping_response(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0x1f,0x24,0x21,0x17,0x20,0x5b};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0x3f, msg.command);
    TEST_ASSERT_EQUAL(1, msg.length);
    TEST_ASSERT_EQUAL(1, msg.type);
    
}
void test_phev_core_my18_xor_decodeMessage_ping_response_even_xor(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0x86,0xbd,0xb8,0xf9,0xb9,0x3d};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0x3f, msg.command);
    TEST_ASSERT_EQUAL(1, msg.length);
    TEST_ASSERT_EQUAL(1, msg.type);
    
}
void test_phev_core_my18_xor_decodeMessage_send_request_even_xor(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0x4f,0x26,0x20,0x23,0x21,0x31,0x43,0xcd};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0x6f, msg.command);
    TEST_ASSERT_EQUAL(3, msg.length);
    TEST_ASSERT_EQUAL(0, msg.type);
    
}
void test_phev_core_my18_xor_decodeMessage_bb(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0x6d,0xd2,0xd7,0x76,0xa5,0x05};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0xbb, msg.command);
    TEST_ASSERT_EQUAL(1, msg.length);
    TEST_ASSERT_EQUAL(1, msg.type);
    
}
void test_phev_core_my18_xor_decodeMessage_cc(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0x1a,0xd2,0xd7,0x80,0xa5,0x4c};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0xcc, msg.command);
    TEST_ASSERT_EQUAL(1, msg.length);
    TEST_ASSERT_EQUAL(1, msg.type);
    
}
void test_phev_core_my18_xor_decodeMessage_long_send(void)
{
    //        uint8_t xor = arr[i + 2] & 0xfe;
    //        uint8_t invert = arr[i] & 0x01;
    //        uint8_t command = (arr[i] ^ xor) | !invert;
    //        length = ((uint8_t) (arr[i + 1] ^ xor)) ^ !invert;
    //        uint8_t type = ((command & 1) ^ !(arr[i + 2] ^ xor));
    //        uint8_t reg = arr[i + 3] ^ xor;
    //        if(command == 0xcd) length = 4;
    
    const uint8_t my18_msg[] = {0xd6,0xae,0xb9,0xac,0xb9,0xf3,0xf4,0xf8,0xe1,0xfd,0xfe,0xfe,0x8b,0xee,0xfe,0xe3,0x89,0x89,0x8b,0x89,0x8a,0x8c,0xb8,0xb8,0x4a};
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0x6e, msg.command);
    TEST_ASSERT_EQUAL(19, msg.length);
    TEST_ASSERT_EQUAL(1, msg.type);
    
}
//0xd6,0xae,0xb9,0xac,0xb9,0xf3,0xf4,0xf8,0xe1,0xfd,0xfe,0xfe

void test_phev_core_my18_xor_decodeMessage_4e(void)
{
    
    const uint8_t my18_msg[] = { 0x4e,0x0c,0x00,0x01,0x37,0xc7,0x69,0x15,0x8b,0x61,0x9c,0x8b,0x02,0xec };
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(my18_msg, sizeof(my18_msg), &msg);

    TEST_ASSERT_EQUAL(0x4e, msg.command);
    TEST_ASSERT_EQUAL(9, msg.length);
    TEST_ASSERT_EQUAL(0, msg.type);
    
}
//phev_core_responseHandler
void test_response_handler_4e(void)
{
    uint8_t value[] = { 0x37,0xc7,0x69,0x15,0x8b,0x61,0x9c,0x8b,0x02 };
    phevMessage_t request = {
        .command = 0x4e,
        .length = 12,
        .type = REQUEST_TYPE,
        .reg = 0x01,
        .data = value,  
    };
    phevMessage_t * msg = phev_core_responseHandler(&request);

    TEST_ASSERT_EQUAL(0xe4, msg->command);
    TEST_ASSERT_EQUAL(0x01, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x01, msg->reg);
    TEST_ASSERT_EQUAL(0, *msg->data);   
} 
/*
Command f6 Type 1 Register 2c Length 4
Orig af a1 53 56 97 57}
Command f7 Type 0 Register c1 Length 5
Orig ec 47 b0 b4 b0 b4}
*/
void test_phev_core_xor_message_even_xor_response(void)
{
    uint8_t input[] = { 0xf6,0x04,0x01,0xc0,0x00,0xbb };
    uint8_t expected[] = { 0xa1,0x53,0x56,0x97,0x57,0xec };
    uint8_t xor = 0x56;

    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XOROutboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
}  
void test_phev_core_xor_message_odd_xor_response(void)
{
    uint8_t input[] = { 0xf6,0x04,0x01,0x01,0x00,0xfc };
    uint8_t expected[] = { 0x42,0xb0,0xb5,0xb5,0xb4,0x48 };
    uint8_t xor = 0xb5;

    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XOROutboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
}  
/*
Command f7 Type 1 Register b Length 5
Orig bb 49 4d 47 4c 48
Command f7 Type 1 Register b Length 5
Orig c9 3b 3f 35 3d 39
*/
void test_phev_core_xor_message_even_xor_request(void)
{
    uint8_t input[] = { 0xf6,0x04,0x00,0x0a,0x01,0x05};
    uint8_t expected[] = { 0xbb,0x49,0x4d,0x47,0x4c,0x48 };
    //uint8_t xor = 0x4d;
    uint8_t xor = phev_core_getXOR(expected);
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XOROutboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 
void test_phev_core_xor_message_odd_xor_request(void)
{
    uint8_t input[] = { 0xf6,0x04,0x00,0x0a,0x02,0x06 };
    uint8_t expected[] = { 0xc9,0x3b,0x3f,0x35,0x3d,0x39 };
    //uint8_t xor = 0x3f;
    uint8_t xor = phev_core_getXOR(expected);
    
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XOROutboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 
void test_phev_core_xor_inbound_message_odd_xor_request(void)
{
    uint8_t input[] = { 0x4f,0x26,0x20,0x23,0x21,0x31,0x43,0xcd };
    uint8_t expected[] = { 0x6f,0x06,0x00,0x03,0x01,0x11,0x63, 0xed};
    //uint8_t xor = 0x20;
    uint8_t xor = phev_core_getXOR(input) &0xfe;
    
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XORInboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 
void test_phev_core_xor_inbound_message_odd_ping(void)
{
    uint8_t input[] = { 0x1f,0x24,0x21,0x17,0x20,0x5b }; 
    uint8_t expected[] = { 0x3f,0x04,0x01,0x37,0x00,0x7b };
    //uint8_t xor = 0x20;
    uint8_t xor = phev_core_getXOR(input) & 0xfe;
    
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XORInboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 
//cf a4 a0 ab a1 df
void test_phev_core_xor_inbound_6f_resp(void)
{
    uint8_t input[] = { 0x62,0x09,0x0d,0x2c,0x0d,0x99 }; 
    uint8_t expected[] = { 0x6f,0x04,0x00,0x21,0x00,0x94 };
    //uint8_t xor = 0x0d;
    uint8_t xor = phev_core_getXOR(input);
    
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XORInboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 
// a09b9ec09f3c
void test_phev_core_xor_inbound_ping_even_resp(void)
{
    uint8_t input[] = { 0xa0,0x9b,0x9e,0xc0,0x9f,0x3c }; 
    uint8_t expected[] = { 0x3f,0x04,0x01,0x5f,0x00,0xa3 };
    //uint8_t xor = 0x9f;
    uint8_t xor = phev_core_getXOR(input);
    
    message_t * message = msg_utils_createMsg(input, sizeof(input));
    message_t * encoded = phev_core_XORInboundMessage(message,xor);

    TEST_ASSERT_NOT_NULL(encoded);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,encoded->data,sizeof(expected));
       
} 

//78 43 46 25 47 e1