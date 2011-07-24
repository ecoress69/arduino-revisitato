#define _UNIT_TEST_

#include <TemperatureProfileManager.h>
#include <TemperatureProfile.h>
#include <ArduinoUnit.h>
#include <EEPROM.h>
#include <Time.h>
#include <string.h>

TestSuite suite;

#define MEM_ADDR 0x100
#define NUM_PROFILES 5


void setup() {
  Serial.begin(9600);
  int i;
  TemperatureProfileManager::setMemoryInfo(MEM_ADDR, NUM_PROFILES);

  for(i = 0; i < MEM_ADDR; i++) {
    EEPROM.write(i, -22);
    EEPROM.write(i + MEM_ADDR + sizeof(TemperatureProfile) * NUM_PROFILES, -33);
  }
  TPM.format();
}

void loop() {
  suite.run();
}

test(integrity) {
  assertEquals(0, checkIntegrity());
  assertEquals(5, TPM.maxNumOfProfiles());
  assertEquals(5, TPM.freeSpace());
}

test(oneProfileTest) {
  TPROFILE.setId(1);
  TPROFILE.setName("ONE P TEST");
  setProfile(10, 20);
  TPM.save();
 
  assertTrue(TPM.exists(1));
  TPROFILE.setId(-1);
  TPROFILE.setName("EMPTY");
  
  TPM.load(1);
  assertEquals(1, TPROFILE.getId());
  assertEquals(0, strncmp(TPROFILE.getName(), "ONE P TEST", 11));
  
  checkProfile(10, 20);
}

test(spaceFull) {
  int i;
  char buffer[12];
  
  TPM.format();
  for(i = 0; i < TPM.maxNumOfProfiles(); i++) {
    TPROFILE.setId(i);
    sprintf(buffer, "FULL %d", i);
    TPROFILE.setName(buffer);
    setProfile(i * 3,  i * 4 );
    assertTrue(TPM.save());
  }
  
  // Let's do some basic checks
  assertEquals(0, checkIntegrity());
  assertEquals(0, TPM.freeSpace());
  assertEquals(TPM.maxNumOfProfiles(), TPM.used());
  
  // All profiles should exist
  for(i = 0; i < TPM.maxNumOfProfiles(); i++) {
    assertTrue(TPM.exists(i));
  }
  
  // Adding another new profile should fail
  TPROFILE.setId(11);
  TPROFILE.setName("FULL 11");
  assertTrue(!TPM.save());
  assertTrue(!TPM.exists(11));

  // Now we are removing a profile from the middle
  // All we got to do is set the profile id
  assertTrue(TPM.exists(3));

  TPROFILE.setId(3);
  TPM.remove();
  
  assertEquals(0, checkIntegrity());
  assertEquals(1, TPM.freeSpace());
  assertTrue(!TPM.exists(3));
  for(i = 0; i < TPM.maxNumOfProfiles(); i++) {
    if(i != 3) {
      assertTrue(TPM.exists(i));
    }
  }

  
  // Now we add a new profile 
  TPROFILE.setId(12);
  TPROFILE.setName("FULL 12");
  setProfile(36, 48);
  TPM.save();
  
  assertEquals(0, checkIntegrity());
  assertEquals(0, TPM.freeSpace());
  for(i = 0; i < TPM.maxNumOfProfiles(); i++) {
    if(i != 3) {
      assertTrue(TPM.exists(i));
    }
  }
  assertTrue(TPM.exists(2));
  
  // Finally, we will check the content of all profiles
 for(i = 0; i < TPM.maxNumOfProfiles(); i++) {
    if(i != 3) {
      TPM.load(i);
      assertTrue(checkProfile(i * 3, i * 4)); 
    }
  }
  
  TPM.load(12);
  assertTrue(checkProfile(36, 48)); 
  
  TPM.format();
  assertEquals(0, TPM.used());
}


test(removeProfile) {
  TPM.format();
  TPROFILE.setId(1);
  TPROFILE.setName("REMOVE 1");
  setProfile(10,20);
  TPM.save();
  
  TPROFILE.setId(2);
  TPROFILE.setName("REMVOE 2");
  setProfile(20, 30);
  TPM.save();
  
  TPM.load(1);
  TPM.remove();
  
  assertTrue(TPM.exists(2));
  assertTrue(!TPM.exists(1));
  assertEquals(TPM.maxNumOfProfiles() -1, TPM.freeSpace());
  assertEquals(1, TPM.used());
  assertEquals(0, checkIntegrity());
}


test(loadSave) {
  TPM.format();
  
  for(int id = 1; id <= NUM_PROFILES; id++) {
    TPROFILE.setId(id);
    TPROFILE.setName("Work AM/WI");
    TPROFILE.add(id, 100 + id);
    TPROFILE.add(20 + id, 120 + id);
    assertTrue(TPM.save());
  }
  
  for(int id = 1; id <= NUM_PROFILES; id++) {
    assertTrue(TPM.exists(id));
    assertTrue(TPM.load(id));
  }
}

int checkIntegrity() {
  bool pass = 0;
  
  for(int i = 0; i < MEM_ADDR; i++) {
   if(((int8_t)EEPROM.read(i)) != -22) {
     Serial.print("(PRE)Address ");
     Serial.print(i, HEX);
     Serial.print(" has value ");
     Serial.println((int8_t)EEPROM.read(i), DEC); 
     pass = i;
     break;
   }
   if(((int8_t)EEPROM.read(i +  MEM_ADDR + sizeof(TemperatureProfile) * NUM_PROFILES)) != -33) {
     pass = i +  MEM_ADDR + sizeof(TemperatureProfile) * NUM_PROFILES;
     Serial.print("(POST)Address ");
     Serial.print(i +  MEM_ADDR + sizeof(TemperatureProfile) * NUM_PROFILES, HEX);
     Serial.print(" has value ");
     Serial.println((int8_t)EEPROM.read(i), DEC); 
     break;
   }
  }
  
  return pass;
}

void setProfile(int timeOffset, int tempOffset) {
  for(int i = 0; i < TemperatureProfile::MAX_SIZE; i++) {
    TPROFILE.add(i + timeOffset, i + tempOffset);
  }
}

bool checkProfile(int timeOffset, int tempOffset) {
  int time, temperature;
  bool success = true;
  
  for(int i = 0; i < TemperatureProfile::MAX_SIZE; i++) {
    TPROFILE.getAt(i, time, temperature);
    if(((i + timeOffset) != time) && ((i + tempOffset) != temperature)) {
      Serial.print("Profile check failed. Expected ");
      Serial.print(i+timeOffset);  
      Serial.print("/");
      Serial.print(i+tempOffset);
      Serial.print(" but got");
      Serial.print(time);  
      Serial.print("/");
      Serial.println(temperature);
      success = false;
    } 
  }
  
  return success;
}

