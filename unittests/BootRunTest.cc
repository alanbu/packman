
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "libpkg/boot_options_file.h"
#include "fileutils.h"

// Dummy application names used in tests
static const char *test_app = "HostFS:$.!Allocate";
static const char *test_not_app = "HostFS:$.Dev.!Fred"; 
static const char *boot_drive_app = "Boot:^.!PackMan";
static const char *boot_drive_quiz_app = "Boot:^.Diversions.!Quiz";

// Expand boot drive app to full pathname
static std::string test_boot_drive_app = canonicalise(boot_drive_app);
static std::string test_boot_drive_quiz_app = canonicalise(boot_drive_quiz_app);

/**
 * Test fixture to test the boot_run class used for reading/writing
 * The lookup options for RISC OS
 */
class BootRunTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( BootRunTest );

	CPPUNIT_TEST( test_pathname );
	CPPUNIT_TEST( test_section_presence );
	CPPUNIT_TEST( test_contains );
	CPPUNIT_TEST( test_add );
	CPPUNIT_TEST( test_remove );
	CPPUNIT_TEST( test_replace );
	CPPUNIT_TEST( test_boot_no_run );
	CPPUNIT_TEST( test_acorn_desktop );

	CPPUNIT_TEST_SUITE_END();

private:
	pkg::run_options *_boot_run;

public:
	/**
	 * Test fixture setup override
	 */
	void setUp()
	{
		_boot_run = new pkg::run_options();
	}

	/**
	 * Text fixture tear down override
	 */
	void tearDown()
	{
		delete _boot_run;
	}

	/**
	 * Test the path name is what we expect
	 */
	void test_pathname()
	{
		CPPUNIT_ASSERT(_boot_run->write_pathname() == "<Choices$Write>.Boot.Desktop");
		CPPUNIT_ASSERT(_boot_run->read_pathname() == "Choices:Boot.Desktop");
	}

	/**
	 * Test if presence/absense of look at section
	 */
	void test_section_presence()
	{
		_boot_run->use_test_pathname("<PMUnitTests$Dir>.data.Desktop6");
		_boot_run->rollback(); // Load test file
		CPPUNIT_ASSERT(!_boot_run->has_section());

		_boot_run->use_test_pathname("<PMUnitTests$Dir>.data.Desktop7");
		_boot_run->rollback(); // Load test file
		CPPUNIT_ASSERT(_boot_run->has_section());
	}

	/**
	 * Test contains method
	 */
	void test_contains()
	{
		// Check file which as no look at section
		_boot_run->use_test_pathname("<PMUnitTests$Dir>.data.Desktop6");
		_boot_run->rollback(); // Load test file

		CPPUNIT_ASSERT(!_boot_run->contains(test_app));

		// Check file with one entry
		_boot_run->use_test_pathname("<PMUnitTests$Dir>.data.Desktop7");
		_boot_run->rollback(); // Load test file
		CPPUNIT_ASSERT(!_boot_run->contains(test_app));
		CPPUNIT_ASSERT(!_boot_run->contains(test_not_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_app));

		// Check file with two entries
		_boot_run->use_test_pathname("<PMUnitTests$Dir>.data.Desktop8");
		_boot_run->rollback(); // Load test file

		CPPUNIT_ASSERT(_boot_run->contains(test_app));
		CPPUNIT_ASSERT(!_boot_run->contains(test_not_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_app));

		CPPUNIT_ASSERT(_boot_run->contains_raw(boot_drive_app));
	}

	void test_add()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop6";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopAD";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_boot_run->use_test_pathname(test_file);
		_boot_run->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_boot_run->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->modified());

		// Save it
		_boot_run->commit();
		CPPUNIT_ASSERT(!_boot_run->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop7"));

		CPPUNIT_ASSERT(_boot_run->add(test_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_app));
		CPPUNIT_ASSERT(_boot_run->modified());
		_boot_run->commit();

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop8"));
	}

	void test_remove()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop8";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopRM";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_boot_run->use_test_pathname(test_file);
		CPPUNIT_ASSERT_NO_THROW(_boot_run->rollback()); // Load test file

		// Remove last item
		CPPUNIT_ASSERT(_boot_run->remove(test_boot_drive_app));
		CPPUNIT_ASSERT(!_boot_run->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_app));
		CPPUNIT_ASSERT(_boot_run->modified());
		_boot_run->commit();
		CPPUNIT_ASSERT(!_boot_run->modified());

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop9"));


		// Remove first item
		CPPUNIT_ASSERT(_boot_run->remove(test_app));
		CPPUNIT_ASSERT(!_boot_run->contains(test_app));
		CPPUNIT_ASSERT(_boot_run->modified());

		// Save it
		_boot_run->commit();
		CPPUNIT_ASSERT(!_boot_run->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop10"));
	}

	void test_replace()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop8";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopMV";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_boot_run->use_test_pathname(test_file);
		CPPUNIT_ASSERT_NO_THROW(_boot_run->rollback()); // Load test file

		// Test move on non-boot drive
		CPPUNIT_ASSERT(_boot_run->replace(test_app, test_not_app));
		CPPUNIT_ASSERT(_boot_run->modified());
		CPPUNIT_ASSERT(!_boot_run->contains(test_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_not_app));

		// Test move on boot drive
		CPPUNIT_ASSERT(_boot_run->replace(test_boot_drive_app, test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(!_boot_run->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(_boot_run->contains_raw(boot_drive_quiz_app));

		CPPUNIT_ASSERT_NO_THROW(_boot_run->commit());
		CPPUNIT_ASSERT(compare_files("<PMUnitTests$Dir>.data.Desktop13", test_file));

		CPPUNIT_ASSERT_NO_THROW(_boot_run->rollback()); // Reload from saved file
		CPPUNIT_ASSERT(_boot_run->contains(test_not_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_quiz_app));

		// Move everything back
		CPPUNIT_ASSERT(_boot_run->replace(test_boot_drive_quiz_app, test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->replace(test_not_app, test_app));
		CPPUNIT_ASSERT_NO_THROW(_boot_run->commit());
		
		CPPUNIT_ASSERT(compare_files(data_file, test_file));
	}

	void test_boot_no_run()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop11";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopBNR";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_boot_run->use_test_pathname(test_file);
		CPPUNIT_ASSERT_NO_THROW(_boot_run->rollback()); // Load test file

		CPPUNIT_ASSERT(!_boot_run->has_section()); // Double check file doesn't have a run section

		CPPUNIT_ASSERT(_boot_run->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->modified());

		// Save it
		_boot_run->commit();
		CPPUNIT_ASSERT(!_boot_run->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop12"));
	}
	
	// Test desktop file that has Acorn in the sections instead of RISC OS
	void test_acorn_desktop()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.Desktop14";
		const char *test_file = "<PMUnitTests$Dir>.results.DesktopACR";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_boot_run->use_test_pathname(test_file);
		_boot_run->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_boot_run->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_boot_run->modified());

		// Save it
		_boot_run->commit();
		CPPUNIT_ASSERT(!_boot_run->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.Desktop16"));		
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( BootRunTest );
