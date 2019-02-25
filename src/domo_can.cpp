#include <TrueRandom.h>
#include <SPI.h>
#include "domo_can.h"
#include <Arduino.h>
#include <EEPROM.h>

/**
 * Constructor.
 */
DomoCan::DomoCan() {
  // initialize attributes.
  _relays.nbElements = 0;
}

/**
 * Add a relay in the curent configuration.
 * Must be call befora the function init.
 * 
 * @param setRelay pointer to function to set the relay.
 * @param resetReset pointer to function to reset the relay.
 * @param getStatus pointer to function to get the current status of relay
 */
void DomoCan::addRelay(void (*setRelay)(void), void (*resetRelay)(void), bool (*getStatus)(void)) {
  _relays.nbElements++;
  // realloc memory
  Relay *newArr = new Relay[_relays.nbElements];
  memcpy(newArr, _relays.elements, _relays.nbElements * sizeof(Relay));
  delete[] _relays.elements;
  _relays.elements = newArr;

  // init value of new relay.
  _relays.elements[_relays.nbElements -1] = {setRelay, resetRelay, getStatus};
}

void DomoCan::addBtn(bool (*getStatus)(void)) {
}

/**
 * Initialisation of can.
 * Enable the CAN BUS, check the id if no id wait an id by dhcp server (id:0)
 * Send the current config to the dhcp server.
 */
void DomoCan::init() {
   // set mcp2515 configuration.
  SPI.begin();
  mcp2515 = new MCP2515(10);
  mcp2515->reset();
  mcp2515->setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515->setConfigMode();
  mcp2515->setFilterMask(MCP2515::MASK0, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF0, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF1, true, 0x00000000);
  mcp2515->setFilterMask(MCP2515::MASK1, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF2, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF3, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF4, true, 0x00000000);
  mcp2515->setFilter(MCP2515::RXF5, true, 0x00000000);
  mcp2515->setNormalMode();
  
  // get the current addresse in eeprom.
  EEPROM.get(0, _eeprom);
  if( _eeprom.crc == crc16()) { 
    dhcp(getDhcpFromEeprom());
  } else {
    // first launch th crc16 is not valid.
    dhcp(generateDhcpRequest());
  }
}

void DomoCan::dhcp(struct can_frame dhcpRequest) {
  // the tmp id for DHCP.
  unsigned long tmpId = (((unsigned long)dhcpRequest.data[1]) << 24) | (((unsigned long)dhcpRequest.data[2]) << 16) | (((unsigned long)dhcpRequest.data[3]) << 8) | (unsigned long)dhcpRequest.data[4];
  // send a DHCP request.
  mcp2515->sendMessage(&dhcpRequest);
  // while response.
  struct can_frame canMsg;
  unsigned long start = millis();
  MCP2515::ERROR error;
  do {
    error = mcp2515->readMessage(&canMsg);
    // wait 10s
    Serial.println((millis() - start));
  } while((canMsg.can_id & 0x1FFFFFFF) != tmpId & (millis() - start) < 1000);
  // if a response is receive generate an other id.
  if(error == MCP2515::ERROR_OK && canMsg.data[0] == DHCP_DATA0_ID_IS_NOT_DISPONIBLE) {
    dhcp(generateDhcpRequest());
  } else {
    _eeprom.id = tmpId;
    _eeprom.crc = crc16();
    EEPROM.put(0, _eeprom);
  }
}

struct can_frame DomoCan::generateDhcpRequest() {
  struct can_frame dhcpRequest;
  dhcpRequest.can_id  = 0x00000000 | CAN_EFF_FLAG;
  dhcpRequest.can_dlc = 5;
  dhcpRequest.data[0] = MESSAGE_TYPE_DHCP;
  dhcpRequest.data[1] = TrueRandom.randomByte() & 0x1F;
  dhcpRequest.data[2] = TrueRandom.randomByte();
  dhcpRequest.data[3] = TrueRandom.randomByte();
  dhcpRequest.data[4] = TrueRandom.randomByte();
  return dhcpRequest;
}

struct can_frame DomoCan::getDhcpFromEeprom() {
  struct can_frame dhcpRequest;
  dhcpRequest.can_id  = 0x00000000 | CAN_EFF_FLAG;
  dhcpRequest.can_dlc = 5;
  dhcpRequest.data[0] = MESSAGE_TYPE_DHCP;
  dhcpRequest.data[1] = ((_eeprom.id >> 24) & 0xFF);
  dhcpRequest.data[2] = ((_eeprom.id >> 16) & 0xFF);
  dhcpRequest.data[3] = ((_eeprom.id >> 8) & 0xFF);
  dhcpRequest.data[4] = ((_eeprom.id) & 0xFF);
  return dhcpRequest;
}

/**
 * CRC for check the value on EEPROM
 */
uint16_t DomoCan::crc16() {
  uint16_t crc = 0xFFFF;
  for (unsigned int i = 0; i < 2; ++i) {
    uint16_t dbyte = ((char *)_eeprom.id)[i];
    crc ^= dbyte << 8;
    for (unsigned char j = 0; j < 8; ++j) {
      uint16_t mix = crc & 0x8000;
      crc = (crc << 1);
      if (mix) {
        crc = crc ^ 0x1021;
      }
    }
  }
  return crc;
}
