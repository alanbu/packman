#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

/**
 * Main entry point to run the tests
 */
int main( int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  // First, we retreive the instance of the TestFactoryRegistry:
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  //Then, we obtain and add a new TestSuite created by the TestFactoryRegistry that contains all the test suite registered using CPPUNIT_TEST_SUITE_REGISTRATION().
  runner.addTest( registry.makeTest() );
  runner.run();

  return 0;
}
