#include "stdafx.h"
#include "CppUnitTest.h"
#include "ElevatorWnd.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Testing
{
	TEST_CLASS(CElevatorWndTest)
	{
	public:
        BEGIN_TEST_METHOD_ATTRIBUTE(TestCElevatorWndTimer)
            TEST_TRAIT(L"ElevatorLib")
        END_TEST_METHOD_ATTRIBUTE()

        /*
         * Test that the user timer works
         */
		TEST_METHOD(TestCElevatorWndTimer)
		{
			// Create a test object
            CElevatorWnd elevator;

            // Run it a bit
            elevator.Update(1);

            // Reset the timer
            elevator.ResetTimer();

            Assert::AreEqual(0.0, elevator.GetTimer(), 0.001);

            // And test timing
            elevator.Update(0.234);
            Assert::AreEqual(0.234, elevator.GetTimer(), 0.001);
		}

        BEGIN_TEST_METHOD_ATTRIBUTE(TestCElevatorWndDoor)
            TEST_TRAIT(L"ElevatorLib")
        END_TEST_METHOD_ATTRIBUTE()

        /*
         * Test that the elevator doors work correctly 
         */
        TEST_METHOD(TestCElevatorWndDoor)
        {
			// Create a test object
            CElevatorWnd elevator;

            for (int floor = 1; floor <= CElevatorWnd::NumFloors; floor++)
            {
                Assert::IsTrue(elevator.IsDoorClosed(floor));

                elevator.SetDoorMotor(floor, 1);
                elevator.Update(1.1);     // Enough time to open door
            
                Assert::IsTrue(elevator.IsDoorOpen(floor));

                elevator.SetDoorMotor(floor, -1);
                elevator.Update(1.1);     // Enough time to close door
            
                Assert::IsTrue(elevator.IsDoorClosed(floor));
            }

        }



	};
}