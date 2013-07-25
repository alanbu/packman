
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

#include "look_at.h"
#include "fileutils.h"

// Dummy application names used in tests
static const char *test_app = "HostFS:$.!Allocate";
static const char *test_not_app = "HostFS:$.Dev.!Fred"; 
static const char *test_quiz_app = "HostFS:$.Games.!Quiz";
static const char *boot_drive_app = "Boot:^.!PackMan";
static const char *boot_drive_quiz_app = "Boot:^.Diversions.!Quiz";

// Expand boot drive app to full pathname
static std::string test_boot_drive_app = canonicalise(boot_drive_app);
static std::string test_boot_drive_quiz_app = canonicalise(boot_drive_quiz_app);

/**
 * Test fixture to test the add_to_apps class used for reading/writing
 * the applications to be added to the virtual RISC OS application directory
 */
class BootAddToApps : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( BootAddToApps );

	CPPUNIT_TEST( test_path_name );
	CPPUNIT_TEST( test_section_presence );
	CPPUNIT_TEST( test_contains );
	CPPUNIT_TEST( test_add );
	CPPUNIT_TEST( test_remove );
	CPPUNIT_TEST( test_replace );

	CPPUNIT_TEST_SUITE_END();

private:
	add_to_apps_options *_add_to_apps;

public:
	/**
	 * Test fixture setup override
	 */
	void setUp()
	{
		_add_to_apps = new add_to_apps_options();
	}

	/**
	 * Text fixture tear down override
	 */
	void tearDown()
	{
		delete _add_to_apps;
	}

	/**
	 * Test the path name is what we expect
	 */
	void test_path_name()
	{
		CPPUNIT_ASSERT(_add_to_apps->path_name() == "<Choices$Write>.Boot.PreDesktop");
	}

	/**
	 * Test if presence/absense of look at section
	 */
	void test_section_presence()
	{
		_add_to_apps->use_test_path_name("<PMUnitTests$Dir>.data.PreDesktop1");
		_add_to_apps->rollback(); // Load test file
		CPPUNIT_ASSERT(!_add_to_apps->has_section());

		_add_to_apps->use_test_path_name("<PMUnitTests$Dir>.data.PreDesktop2");
		_add_to_apps->rollback(); // Load test file
		CPPUNIT_ASSERT(_add_to_apps->has_section());
	}

	/**
	 * Test contains method
	 */
	void test_contains()
	{
		// Check file which as no look at section
		_add_to_apps->use_test_path_name("<PMUnitTests$Dir>.data.PreDesktop1");
		_add_to_apps->rollback(); // Load test file

		CPPUNIT_ASSERT(!_add_to_apps->contains(test_app));

		// Check file with one entry
		_add_to_apps->use_test_path_name("<PMUnitTests$Dir>.data.PreDesktop2");
		_add_to_apps->rollback(); // Load test file
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_app));
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_not_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_app));

		// Check file with two entries
		_add_to_apps->use_test_path_name("<PMUnitTests$Dir>.data.PreDesktop3");
		_add_to_apps->rollback(); // Load test file

		CPPUNIT_ASSERT(_add_to_apps->contains(test_app));
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_not_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_app));

		CPPUNIT_ASSERT(_add_to_apps->contains_raw(boot_drive_app));
	}

	void test_add()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.PreDesktop1";
		const char *test_file = "<PMUnitTests$Dir>.results.PreDesktopAD";

		// Initialise test file
		copy_file(data_file, test_file);
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_add_to_apps->use_test_path_name(test_file);
		_add_to_apps->rollback(); // Load test file
		
		// Add first item
		CPPUNIT_ASSERT(_add_to_apps->add(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->modified());

		// Save it
		_add_to_apps->commit();
		CPPUNIT_ASSERT(!_add_to_apps->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.PreDesktop2"));

		CPPUNIT_ASSERT(_add_to_apps->add(test_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_app));
		CPPUNIT_ASSERT(_add_to_apps->modified());
		_add_to_apps->commit();

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.PreDesktop3"));
	}

	void test_remove()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.PreDesktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.PreDesktopRM";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_add_to_apps->use_test_path_name(test_file);
		CPPUNIT_ASSERT_NO_THROW(_add_to_apps->rollback()); // Load test file

		// Remove last item
		CPPUNIT_ASSERT(_add_to_apps->remove(test_app));
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->modified());
		_add_to_apps->commit();
		CPPUNIT_ASSERT(!_add_to_apps->modified());

		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.PreDesktop2"));


		// Remove first item
		CPPUNIT_ASSERT(_add_to_apps->remove(test_boot_drive_app));
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->modified());

		// Save it
		_add_to_apps->commit();
		CPPUNIT_ASSERT(!_add_to_apps->modified());
		CPPUNIT_ASSERT(compare_files(test_file, "<PMUnitTests$Dir>.data.PreDesktop5"));
	}

	void test_replace()
	{
		const char *data_file = "<PMUnitTests$Dir>.data.PreDesktop3";
		const char *test_file = "<PMUnitTests$Dir>.results.PreDesktopMV";

		// Initialise test file
		CPPUNIT_ASSERT_NO_THROW(copy_file(data_file, test_file));
		// Double check copy worked
		CPPUNIT_ASSERT(compare_files(data_file, test_file));

		_add_to_apps->use_test_path_name(test_file);
		CPPUNIT_ASSERT_NO_THROW(_add_to_apps->rollback()); // Load test file

		// Test move on non-boot drive
		CPPUNIT_ASSERT(_add_to_apps->replace(test_app, test_not_app));
		CPPUNIT_ASSERT(_add_to_apps->modified());
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_not_app));

		// Test move on boot drive
		CPPUNIT_ASSERT(_add_to_apps->replace(test_boot_drive_app, test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(!_add_to_apps->contains(test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_quiz_app));
		CPPUNIT_ASSERT(_add_to_apps->contains_raw(boot_drive_quiz_app));

		CPPUNIT_ASSERT_NO_THROW(_add_to_apps->commit());
		CPPUNIT_ASSERT(compare_files("<PMUnitTests$Dir>.data.PreDesktop7", test_file));

		CPPUNIT_ASSERT_NO_THROW(_add_to_apps->rollback()); // Reload from saved file
		CPPUNIT_ASSERT(_add_to_apps->contains(test_not_app));
		CPPUNIT_ASSERT(_add_to_apps->contains(test_boot_drive_quiz_app));

		// Move everything back
		CPPUNIT_ASSERT(_add_to_apps->replace(test_boot_drive_quiz_app, test_boot_drive_app));
		CPPUNIT_ASSERT(_add_to_apps->replace(test_not_app, test_app));
		CPPUNIT_ASSERT_NO_THROW(_add_to_apps->commit());
		
		CPPUNIT_ASSERT(compare_files(data_file, test_file));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( BootAddToApps );
