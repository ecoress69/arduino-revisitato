#define _UNIT_TEST_

#include <ArduinoUnit.h>
#include <Time.h>
#include <EEPROM.h>

#include <TemperatureProfile.h>
#include <TemperatureProfileManager.h>
#include <TemperatureManager.h>

TestSuite suite;

#define MEM_ADDR 0x100
#define NUM_PROFILES 14

#define toSeconds(hours, minutes) hours * 3600L +  minutes * 60L
void setup() {
  Serial.begin(9600);
  TemperatureProfileManager::setMemoryInfo(MEM_ADDR + 0x50, NUM_PROFILES);
  TemperatureManager::setMemoryInfo(MEM_ADDR);
  
  TEMPMGR.setTimeOfYear(TemperatureManager::SUMMER);
  // We need to set up the different profiles, we use 8
  TPM.format();

  for(int id = 1; id <= NUM_PROFILES; id++) {
    TPROFILE.setId(id);
    TPROFILE.setName("Work AM/WI");
    TPROFILE.add(id, 50 + id);
    TPROFILE.add(24 + id, 80 + id);
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
  TEMPMGR.setProfile(7, TemperatureManager::WEDNESDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(8, TemperatureManager::WEDNESDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(9, TemperatureManager::THURSDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(10, TemperatureManager::THURSDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(11, TemperatureManager::FRIDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(12, TemperatureManager::FRIDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(13, TemperatureManager::SATURDAY, TemperatureManager::AM, TemperatureManager::SUMMER);
  TEMPMGR.setProfile(14, TemperatureManager::SATURDAY, TemperatureManager::PM, TemperatureManager::SUMMER);
  // This will for to re-read the info from the EEPROM
  TemperatureManager::setMemoryInfo(0);
  TemperatureManager::setMemoryInfo(MEM_ADDR);
}

void loop() {
  suite.run();
}

test(basic) {
  assertEquals(1, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::AM, TemperatureManager::SUMMER));
  assertEquals(2, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::PM, TemperatureManager::SUMMER));
  assertEquals(-1, TEMPMGR.getProfile(TemperatureManager::SUNDAY, TemperatureManager::PM, TemperatureManager::WINTER));
  assertEquals(13, TEMPMGR.getProfile(TemperatureManager::SATURDAY, TemperatureManager::AM, TemperatureManager::SUMMER));
  assertEquals(14, TEMPMGR.getProfile(TemperatureManager::SATURDAY, TemperatureManager::PM, TemperatureManager::SUMMER));
}
  
test(temperatureForTime) {
  tmElements_t te;
  time_t time;

  te.Day = 24;
  te.Month = 7;
  te.Year = 41;
  
  te.Hour = 6;
  te.Minute = 14;
  te.Second = 59;  
  time = makeTime(te);
  
  for(int i = 1; i <= NUM_PROFILES; i++) {
    // The first set point of the profile
    time += 1L;
    //temp =  ;
    assertEquals(50 + i, TEMPMGR.getSetPointFor(time));
    
    // Just before second set point
    time += (6L * 3600L - 1L);
    assertEquals(50 + i, TEMPMGR.getSetPointFor(time));
    
    // Exactly the second set point
    time += 1L;
    assertEquals(80 + i, TEMPMGR.getSetPointFor(time));
    
    // Just before the next profile's first setpoint
    time += (6L * 3600L + 15L * 60L - 1L);
    // When we arrived at the last, we wrap around. As 
    // we gain 30 min every day, we will not find the second of the last
    // profile, but the first of the first profile
    if(i != NUM_PROFILES) {    
      assertEquals(80 + i, TEMPMGR.getSetPointFor(time));
    }
    else {
      assertEquals(51, TEMPMGR.getSetPointFor(time));
    }
  }
}

test(nextChangeTime) {
  tmElements_t te;
  time_t time;
  time_t nextTime;

  te.Day = 24;
  te.Month = 7;
  te.Year = 41;
  te.Hour = 6;
  te.Minute = 15;
  te.Second = 0;
  
  time = makeTime(te);
  nextTime = time - 100L;
  for(int i = 1; i <= NUM_PROFILES; i++) {
    
    // This should return the first set point in the profile
    nextTime = TEMPMGR.nextSetPointChange(nextTime);
    assertUnsignedLongEquals(time, nextTime);
    
    // This points to the first setpoint, so by adding one second, this
    // should point to the second setpoint
    nextTime++;
    time +=  toSeconds(6L, 0L);
    nextTime = TEMPMGR.nextSetPointChange(nextTime);
    assertUnsignedLongEquals(time, nextTime);

    // Again, this should now point to the first set point of the next profile
    nextTime++;    
    time +=  toSeconds(6L, 15L);
  }

}
