#define _UNIT_TEST_

#include <ArduinoUnit.h>
#include <Time.h>
#include <EEPROM.h>

#include <TemperatureProfile.h>
#include <TemperatureProfileManager.h>
#include <TemperatureManager.h>

TestSuite suite;

#define MEM_ADDR 0x100
#define NUM_PROFILES 10

#define addMinutes(minutes) minutes * 60L
void setup() {
 Serial.begin(9600);
  int i;
  TemperatureProfileManager::setMemoryInfo(MEM_ADDR + 0x50, NUM_PROFILES);
  TemperatureManager::setMemoryInfo(MEM_ADDR);
  
  TEMPMGR.setTimeOfYear(TemperatureManager::SUMMER);
  // We need to set up the different profiles, we use 8
  TPM.format();

  for(int id = 1; id < 9; id++) {
    TPROFILE.setId(id);
    TPROFILE.setName("Work AM/WI");
    TPROFILE.add(id, 50 + id);
    TPROFILE.add(20 + id, 80 + id);
    if(!TPM.save()) {
      Serial.print("Failed to save profile: ");
      Serial.println(id);
    }
  }
  
  TEMPMGR.clear();
  
  TEMPMGR.setProfile(1, TemperatureManager::SUNDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(2, TemperatureManager::SUNDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(3, TemperatureManager::MONDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(4, TemperatureManager::MONDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(5, TemperatureManager::TUESDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(6, TemperatureManager::TUESDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(3, TemperatureManager::WEDNESDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(4, TemperatureManager::WEDNESDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(5, TemperatureManager::THURSDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(6, TemperatureManager::THURSDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(3, TemperatureManager::FRIDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(4, TemperatureManager::FRIDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(7, TemperatureManager::SATURDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(8, TemperatureManager::SATURDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
}

void loop() {
  suite.run();
}

test(basic) {
  assertEquals(1, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::AM, TemperatureManager::SUMMER));
  assertEquals(2, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::PM, TemperatureManager::SUMMER));
  assertEquals(-1, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::PM, TemperatureManager::WINTER));
  assertEquals(7, TEMPMGR.getProfile(TemperatureManager::SATURDAY, TemperatureManager::AM, TemperatureManager::SUMMER));
  assertEquals(8, TEMPMGR.getProfile(TemperatureManager::SATURDAY, TemperatureManager::PM, TemperatureManager::SUMMER));
}
  
test(temperatureForTime) {
  tmElements_t te;
  time_t time;

  te.Day = 24;
  te.Month = 7;
  te.Year = 41;
  te.Hour = 5;
  te.Minute = 0;
  te.Second = 0;
  
  time = makeTime(te);
  assertEquals(88, TEMPMGR.getSetPointFor(time));
  assertEquals(8, TPROFILE.getId());
  assertUnsignedLongEquals(time + toSeconds(1L, 15L), TEMPMGR.nextSetPointChange(time));
}
