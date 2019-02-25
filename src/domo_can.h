#ifndef _domo_can_h_
#define _domo_can_h_

#include <mcp2515.h>

#define MESSAGE_TYPE_DHCP (0x01)
#define DHCP_DATA0_ID_IS_NOT_DISPONIBLE (0x01)

#define MESSAGE_TYPE_CONFIG (0x02)
#define CONFIG_DATA0_RELAY (0x01)
#define CONFIG_DATA0_BTN (0x02)

typedef struct Eeprom Eeprom;
struct Eeprom {
  unsigned long id;
  uint16_t crc;
};

typedef struct Relay Relay;
struct Relay {
  void (*setRelay)(void);
  void (*resetRelay)(void);
  bool (*getStatus)(void);
};

typedef struct ArrayRelay ArrayRelay;
struct ArrayRelay {
  Relay *elements;
  int nbElements;
};

class DomoCan
{
  public:
    DomoCan();
    void addRelay(void (*setRelay)(void), void (*resetRelay)(void), bool (*getStatus)(void));
    void addBtn(bool (*getStatus)(void));
    void init();
  private:
    uint16_t crc16();
    Eeprom _eeprom;
    ArrayRelay _relays;
    MCP2515 *mcp2515;
    void dhcp(struct can_frame dhcpRequest);
    struct can_frame generateDhcpRequest();
    struct can_frame getDhcpFromEeprom();
};

#endif
