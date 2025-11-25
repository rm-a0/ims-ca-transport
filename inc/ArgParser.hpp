/**
 * @file ArgParser.hpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <string>

/**
 * @class ArgParser
 * @brief Parses and stores command line arguments
 */
class ArgParser {
public:
    /**
     * @brief Constructor for ArgParser class
     * @param argc Argument count
     * @param argv List of arguments
     */
    ArgParser(size_t argc, char* argv[]);

    /**
     * @brief Parses command line arguments
     * @return True if no issues, False if there was an error
     */
    bool parse(void);

    /**
     * @brief Displays help message (expected arguments)
     */
    void displayHelp(void);

    /**
     * @brief Getters
     */
    bool isVizEnabled() const { return vizFlag; }
    std::string getVizDir() const { return vizDir; }
    int getSteps() const { return steps; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    size_t argc;                ///< Argument count
    char** argv;                ///< Arguments
    bool vizFlag = false;       ///< Flag for visualization
    std::string vizDir = "viz"; ///< Directory where PPM output is saved
    int steps = 10;             ///< Number of steps
    int width = 10;             ///< Grid width
    int height = 10;            ///< Grid height
};

#endif // ARG_PARSER_HPP__