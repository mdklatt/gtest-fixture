/**
 * Interface for the sample library module.
 *
 * @file
 */
#ifndef GOOGLETEST_FIXTURE_MODULE_HPP
#define GOOGLETEST_FIXTURE_MODULE_HPP


namespace testing::fixture {
    /**
     * Sample class.
     */
    class SampleClass {
    public:
        /**
         * Construct a SampleClass object.
         *
         * @param num any integer value
         */
        SampleClass(int num=0) : num{num} {}

        /**
         * Add a number.
         *
         * @param x any integer value
         * @return the sum of num and x.
         */
        int add(int x) const;
        
    private:
        const int num;
    };

    /**
     * Sample function.
     *
     * @param x any integer value
     * @param y any integer value
     * @return the sum of x and y
     */
    int add(int x, int y);
}  // namespace testing::fixture

#endif  // GOOGLETEST_FIXTURE_MODULE_HPP 
