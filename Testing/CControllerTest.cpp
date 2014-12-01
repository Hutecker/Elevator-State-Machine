#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include "ElevatorWnd.h"
#include "Controller.h";

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace Testing
{
	TEST_CLASS(CControllerTest)
	{
	public:
		
		TEST_METHOD(TestCControllerSetController)
		{
			// Create a test object
			CElevatorWnd elevator;

			// Create a mock controller and install it
			auto controller = std::make_shared<CController>();

			elevator.SetController(controller);
			Assert::IsTrue(controller->GetElevator() == &elevator);
		}

		TEST_METHOD(TestCControllerDoorMachine)
		{
			// Create a test object
			CElevatorWnd elevator;

			// Create a mock controller and install it
			auto controller = std::make_shared<CController>();

			elevator.SetController(controller);
			Assert::IsTrue(controller->GetElevator() == &elevator);

			int floor = 1;

			// Do the door sequence twice
			for (int i = 0; i < 2; i++)
			{
				// Door should be initially closed
				Assert::IsTrue(elevator.IsDoorClosed(floor));

				// Press open 
				elevator.PressOpen();

				// It takes about 1 second for the door to open, so 
				// run the elevator for a bit more than that
				elevator.Update(1.1);

				Assert::IsTrue(elevator.IsDoorOpen(floor));

				// Press close
				elevator.PressClose();

				// It takes about 1 second for the door to close, so 
				// run the elevator for a bit more than that
				elevator.Update(1.1);

				Assert::IsTrue(elevator.IsDoorClosed(floor));
			}

			TEST_METHOD(TestCControllerDoorMachine2)
			{
				// Create a test object
				CElevatorWnd elevator;

				// Create a mock controller and install it
				auto controller = std::make_shared<CController>();

				elevator.SetController(controller);
				Assert::IsTrue(controller->GetElevator() == &elevator);

				int floor = 1;

				// Do the door sequence twice
				for (int i = 0; i < 2; i++)
				{
					// Door should be initially closed
					Assert::IsTrue(elevator.IsDoorClosed(floor));

					// Press open 
					elevator.PressOpen();

					// It takes about 1 second for the door to open, so 
					// run the elevator for a bit more than that
					elevator.Update(1.1);

					Assert::IsTrue(elevator.IsDoorOpen(floor));

					// Press close
					elevator.PressClose();

					// It takes about 1 second for the door to close, so 
					// run the elevator for a bit more than that
					elevator.Update(1.1);

					Assert::IsTrue(elevator.IsDoorClosed(floor));
				}
		}
	};
}