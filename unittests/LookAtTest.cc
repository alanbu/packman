/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************************/

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "libpkg/boot_options_file.h"
#include "fileutils.h"

// Dummy application names used in tests
static const char *test_app = "HostFS:$.Dev.!GCC";
static const char *test_not_app = "HostFS:$.Dev.!Fred"; 
static const char *test_quiz_app = "HostFS:$.Games.!Quiz";
static const char *boot_drive_app = "Boot:^.!PackMan";
static const char *boot_drive_quiz_app = "Boot:^.Diversions.!Quiz";

// Expand boot drive app to full pathname
static std::string test_boot_drive_app = canonicalise(boot_drive_app);
static std::string test_boot_drive_quiz_app = canonicalise(boot_drive_quiz_app);

/**
 * Test fixture to test the look_at class used for reading/writing
 * The lookup options for RISC OS
 */
class LookAtTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( LookAtTest );

	CPPUNIT_TEST( test_pathname );
	CPPUNIT_TEST( test_section_presence );
	CPPUNIT_TEST( test_contains );
	CPPUNIT_TEST( test_add );
	CPPUNIT_TEST( test_remove );
	CPPUNIT_TEST( test_replace );
	CPPUNIT_TEST( test_acorn_desktop );

	CPPUNIT_TEST_SUITE_END();

private:
	pkg::look_at_options *_look_at;

public:
	/**
	 * Test fixture setup override
	 */
	void setUp()
	{
		_look_at = new pkg::look_at_options();
	}

	/**
	 * Text fixture tear down override
	 */
	void tearDown()
	{
		delete _look_at;
	}

	/**
	 * Test the path name is what we expect
	 */
	void test_pathname()
	{
		CPPUNIT_ASSERT(_look_at->write_pathname() == "<Choices$Write>.Boot.Desktop");
		CPPUNIT_ASSERT(_look_at->read_pathname() == "Choices:Boot.Desktop");
	}

	/**
	 * Test if presence/absense of look at section
	 */
	void test_section_presence()
	{
		_look_at->use_test_pathname("<PMUnitTests$Dir>.data.Desktop1");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(!_look_at->has_section());

		_look_at->use_test_pathname("<PMUnitTests$Dir>.data.Desktop2");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(_look_at->has_section());
	}

	/**
	 * Test contains method
	 */
	void test_contains()
	{
		// Check file which as no look at section
		_look_at->use_test_pathname("<PMUnitTests$Dir>.data.Desktop1");
		_look_at->rollback(); // Load test file

		CPPUNIT_ASSERT(!_look_at->contains(test_app));

		// Check file with one entry
		_look_at->use_test_pathname("<PMUnitTests$Dir>.data.Desktop2");
		_look_at->rollback(); // Load test file
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));

		// Check file with two entries
		_look_at->use_test_pathname("<PMUnitTests$Dir>.data.Desktop3");
		_look_at->rollback(); // Load test file

		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_app));

		CPPUNIT_ASSERT(_look_at->contains_raw(boot_drive_app));
	}

	void test_add()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop1";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopAD";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_pathname(test_file);
		_look_at->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_look_at->add(test_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());

		// Save it
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop2"));

		CPPUNIT_ASSERT(_look_at->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());
		_look_at->commit();

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop3"));
	}

	void test_remove()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopRM";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_pathname(test_file);
		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Load test file

		// Remove last item
		CPPUNIT_ASSERT(_look_at->remove(test_boot_drive_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop2"));


		// Remove first item
		CPPUNIT_ASSERT(_look_at->remove(test_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());

		// Save it
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop4"));
	}

	void test_replace()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopMV";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_pathname(test_file);
		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Load test file

		// Test move on non-boot drive
		CPPUNIT_ASSERT(_look_at->replace(test_app, test_not_app));
		CPPUNIT_ASSERT(_look_at->modified());
		CPPUNIT_ASSERT(!_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->contains(test_not_app));

		// Test move on boot drive
		CPPUNIT_ASSERT(_look_at->replace(test_boot_drive_app, test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(!_look_at->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(_look_at->contains_raw(boot_drive_quiz_app));

		CPPUNIT_ASSERT_NO_THROW(_look_at->commit());
		CPPUNIT_ASSERT(compare_files("<PMUnitTests$Dir>.data.Desktop5", test_file));

		CPPUNIT_ASSERT_NO_THROW(_look_at->rollback()); // Reload from saved file
		CPPUNIT_ASSERT(_look_at->contains(test_not_app));
		CPPUNIT_ASSERT(_look_at->contains(test_boot_drive_quiz_app));

		// Move everything back
		CPPUNIT_ASSERT(_look_at->replace(test_boot_drive_quiz_app, test_boot_drive_app));
		CPPUNIT_ASSERT(_look_at->replace(test_not_app, test_app));
		CPPUNIT_ASSERT_NO_THROW(_look_at->commit());
		
		CPPUNIT_ASSERT(compare_files(data_file, test_file));
	}
	
	// Test a desktop file where the sections have Acorn in them instead of RISC OS
	void test_acorn_desktop()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop14";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopAC";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_look_at->use_test_pathname(test_file);
		_look_at->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_look_at->add(test_app));
		CPPUNIT_ASSERT(_look_at->contains(test_app));
		CPPUNIT_ASSERT(_look_at->modified());

		// Save it
		_look_at->commit();
		CPPUNIT_ASSERT(!_look_at->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop15"));
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( LookAtTest );
