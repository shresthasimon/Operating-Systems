/**
 * @file resourceManager.cpp
 *
 * @brief implementation file for resouce class
 *
 */

#include "resourceManager.h"
/**
 * @name resourceStart
 *
 * @detiails initializes all of the resource queues
 *
 * @param config
 */
void Resource::resourceStart(Config &config)
{
    // initialize hard drive queue
	for (int i = 0; i < config.getHarddriveResources(); i++)
	{
		harddrive.push_back(i);
	}
    // initialize keyboard queue
	for (int i = 0; i < config.getKeyboardResources(); i++)
	{
		keyboard.push_back(i);
	}
    // initialize monitor queue
	for (int i = 0; i < config.getMonitorResources(); i++)
	{
		monitor.push_back(i);
	}
    // initialize printer queue
	for (int i = 0; i < config.getPrinterResources(); i++)
	{
		printer.push_back(i);
	}
	// initialize mouse queue
	for (int i = 0; i < config.getMouseResources(); i++)
	{
		mouse.push_back(i);
	}
}
/**
 * @name resourceManagement
 *
 * @details handles the counting for the resouces used
 *
 * @param descriptor
 */
void Resource::resourceManagement(string descriptor)
{
	int index;
    //pushes the resource to the front and moves it to the back after being used
	if (descriptor == "hard drive")
	{
		index = harddrive.front();
		harddrive.pop_front();
		harddrive.push_back(index);
	}
	else if (descriptor == "keyboard")
	{
		index = keyboard.front();
		keyboard.pop_front();
		keyboard.push_back(index);
	}
	else if (descriptor == "mouse")
	{
		index = mouse.front();
		mouse.pop_front();
		mouse.push_back(index);
	}
	else if (descriptor == "monitor")
	{
		index = monitor.front();
		monitor.pop_front();
		monitor.push_back(index);
	}
	else if (descriptor == "printer")
	{
		index = printer.front();
		printer.pop_front();
		printer.push_back(index);
	}
}