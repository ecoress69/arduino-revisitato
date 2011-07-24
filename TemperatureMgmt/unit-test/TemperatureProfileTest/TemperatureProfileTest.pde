#define _UNIT_TEST_

#include <TemperatureProfile.h>
#include <ArduinoUnit.h>
#include <EEPROM.h>

TestSuite suite;

void setup() {

}

void loop() {
  suite.run();
}

test(newProfile) {
  TemperatureProfile profile;
  int time = -2, setPoint = -2, index = -2;
  
  assertTrue(profile.checkIntegrity());

  assertEquals(-1, profile.getId());
  assertEquals(0, profile.size());
  
  profile.setId(1);
  assertEquals(1, profile.getId());
  
  assertEquals(-1, profile.get(4));
  
  profile.getAt(0, time, setPoint);
  assertEquals(-1, time);
  assertEquals(-1, setPoint);
  
  assertEquals(-1, profile.remove(4));
  
}

test(addAndRemoveSingleElement) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  profile.setId(1);
  
  assertEquals(0, profile.add(4, 50));
  assertEquals(1, profile.size());
  assertTrue(profile.checkIntegrity());  
  
  time = setPoint = 0;

  // Make sure the element exists at the first position
  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(50, setPoint);
  assertTrue(profile.checkIntegrity());
  
  // Make sure we can find it by time
  setPoint = profile.get(4);
  assertEquals(50, setPoint);
  assertTrue(profile.checkIntegrity());

  // Remove the element
  assertEquals(0, profile.remove(4));
  assertEquals(0, profile.size());
  assertTrue(profile.checkIntegrity());
}


test(twoElementsAddedInOrder) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  profile.setId(1);
  profile.add(4, 16);
  profile.add(6, 36);
  assertEquals(2, profile.size());
  assertTrue(profile.checkIntegrity());

  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);

  profile.getAt(1, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);
}

test(twoElementsAddedInReverseOrder) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  profile.setId(1);
  profile.add(6, 36);
  profile.add(4, 16);
  assertEquals(2, profile.size());
  assertTrue(profile.checkIntegrity());

  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);

  profile.getAt(1, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);
}

test(insertBetweenTwoElements) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;

  // Setup  
  profile.setId(1);
  profile.add(4, 16);
  profile.add(6, 36);

  profile.getAt(0, time, setPoint);
  profile.getAt(1, time, setPoint);

  // Test
  profile.add(5, 25);
  assertEquals(3, profile.size());
  assertTrue(profile.checkIntegrity());

  profile.getAt(1, time, setPoint);
  assertEquals(5, time);
  assertEquals(25, setPoint);
  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);
  profile.getAt(2, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);
  
}
  
test(insertAtBeginning) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  // Setup
  profile.setId(1);
  profile.add(4, 16);
  profile.add(6, 36);

  // Test  
  profile.add(3, 9);
  assertEquals(3, profile.size());
  assertTrue(profile.checkIntegrity());

  time = setPoint = -1;  
  profile.getAt(0, time, setPoint);
  assertEquals(3, time);
  assertEquals(9, setPoint);
  time = setPoint = -1;
  profile.getAt(1, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);
  time = setPoint = -1;
  profile.getAt(2, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);
  
}

test(insertAtEnd) {
    TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  // Setup
  profile.setId(1);
  profile.add(4, 16);
  profile.add(6, 36);

  // Test  
  profile.add(7, 49);
  assertEquals(3, profile.size());
  assertTrue(profile.checkIntegrity());

  profile.getAt(2, time, setPoint);
  assertEquals(7, time);
  assertEquals(49, setPoint);
  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);
  profile.getAt(1, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);

}

test(indexOf) {
  TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  // Setup
  profile.setId(1);
  profile.add(4, 16);
  profile.add(6, 36);  
  profile.add(8, 64);

  // Test  
  assertEquals(-1, profile.indexOf(5, 0));

  assertEquals(1, profile.indexOf(6, 0));
  assertEquals(1, profile.indexOf(6, -1));
  assertEquals(1, profile.indexOf(6, 1));
  assertEquals(1, profile.indexOf(7, -1));
  assertEquals(1, profile.indexOf(5, 1));
  

  assertEquals(0, profile.indexOf(4, 0));
  assertEquals(0, profile.indexOf(4, -1));
  assertEquals(0, profile.indexOf(4, 1));
  
  assertEquals(0, profile.indexOf(3, 1));
  assertEquals(0, profile.indexOf(5, -1));
  assertEquals(-1, profile.indexOf(3, -1));
  
  assertEquals(2, profile.indexOf(8, 0));
  assertEquals(2, profile.indexOf(8, -1));
  assertEquals(2, profile.indexOf(8, 1));

  assertEquals(2, profile.indexOf(7, 1));
  assertEquals(2, profile.indexOf(9, -1));
  assertEquals(-1, profile.indexOf(9, 1));  
}


test(insertSameInMiddle) {
    TemperatureProfile profile;
  int time = -1, setPoint = -1, index = -1;
  
  // Setup
  profile.setId(1);
  profile.add(4, 16);
  profile.add(5, 25);  
  profile.add(6, 36);

  // Test  
  profile.add(5, 40);
  assertEquals(3, profile.size());
  assertTrue(profile.checkIntegrity());

  profile.getAt(0, time, setPoint);
  assertEquals(4, time);
  assertEquals(16, setPoint);
  profile.getAt(1, time, setPoint);
  assertEquals(5, time);
  assertEquals(40, setPoint);
  profile.getAt(2, time, setPoint);
  assertEquals(6, time);
  assertEquals(36, setPoint);

}

test(clearProfile) {
  TemperatureProfile profile;
  int time = 0, setPoint = 0;
  int index;
    
  // Setup
  profile.setId(1);
  assertEquals(0, profile.add(4, 16));
  assertEquals(1, profile.add(6, 36));
  profile.clear();  
  assertEquals(0, profile.size());
  assertTrue(profile.checkIntegrity());
}

test(fullArrayAddingReverse) {
  TemperatureProfile profile;
  int time = 0, setPoint = 0, index = -5;

  for(int i = TemperatureProfile::MAX_SIZE; i > 0; i--) {
    profile.add(i, i*i);
  }
  
  assertTrue(profile.checkIntegrity());
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());  
  
  // Let's check the elements
  for(int i = 0; i < TemperatureProfile::MAX_SIZE; i++) {
    assertTrue(profile.getAt(i, time, setPoint));
    assertEquals(i+1, time);
    assertEquals((i+1)*(i+1), setPoint);

    setPoint = -1;    
    setPoint = profile.get(i + 1);
    assertEquals((i+1)*(i+1), setPoint);
  }
}

test(removeFromFullArray) {
  TemperatureProfile profile;
  int time = 0, setPoint = 0, index = -5;

  for(int i = TemperatureProfile::MAX_SIZE; i > 0; i--) {
    profile.add(i, i*i);
  }
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());
  assertEquals(4, profile.remove(5));  
  assertTrue(profile.checkIntegrity());
  assertEquals(TemperatureProfile::MAX_SIZE - 1, profile.size());
  
  // Let's check the elements
  for(int i = 0; i < TemperatureProfile::MAX_SIZE; i++) {
    if(i != 5) {
      assertTrue(profile.getAt(i, time, setPoint));
      assertEquals(i+1, time);
      assertEquals((i+1)*(i+1), setPoint);
  
      setPoint = -1;    
      setPoint = profile.get(i + 1);
      assertEquals((i+1)*(i+1), setPoint);
    }
  }
}

test(addToFullArray) {
  TemperatureProfile profile;
  int time = 0, setPoint = 0, index = -5;

  for(int i = TemperatureProfile::MAX_SIZE + 2; i > 2; i--) {
    profile.add(i, i*i);
  }

  // The adding should fail
  assertEquals(-1, profile.add(20, 111));  
  assertTrue(profile.checkIntegrity());

  // This should fail too
  assertEquals(-1, profile.add(1, 111));  
  assertTrue(profile.checkIntegrity());
}

test(replaceInFullArrayNoSizeChange) {
  TemperatureProfile profile;
  int time = 0, setPoint = 0, index = -5;

  for(int i = 1; i <= TemperatureProfile::MAX_SIZE; i++) {
    profile.add(i * 2, i * 4);
  }
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());

  assertEquals(2, profile.replace(6, 15, 30));
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());
  assertEquals(30, profile.get(15))  
  assertEquals(6, profile.indexOf(15));
  assertTrue(profile.checkIntegrity());
  
  // Now replace an entry that does not exist
  // As the array is full, which should fail
  assertEquals(-1, profile.replace(3, 17, 34));
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());
  assertEquals(9, profile.get(3))  
  assertEquals(-1, profile.indexOf(17));
  
  assertTrue(profile.checkIntegrity());

  // Now replace an existing entry that does not exist
  // As the array is full, which should fail
  // This is really calling "add" only
  assertEquals(1, profile.replace(4, 4, 16));
  assertEquals(16, profile.get(4))  
  assertEquals(1, profile.indexOf(4));
  assertEquals(TemperatureProfile::MAX_SIZE, profile.size());
  assertTrue(profile.checkIntegrity());
  
  // Now remove a value and update another
  assertEquals(2, profile.replace(4, 6, 24));
  assertEquals(24, profile.get(6))  
  assertEquals(1, profile.indexOf(6));
  assertEquals(-1, profile.indexOf(4));
  assertEquals(TemperatureProfile::MAX_SIZE - 1, profile.size());
  assertTrue(profile.checkIntegrity());
  
  
}
