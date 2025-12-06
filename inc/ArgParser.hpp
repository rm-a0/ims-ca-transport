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
    bool isPlotEnabled() const { return plotFlag; }
    std::string getVizDir() const { return vizDir; }
    std::string getPlotDir() const { return plotDir; }
    int getSteps() const { return steps; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getVMax() const { return vmax; }
    double getProb() const { return prob; }
    double getDensity() const { return density; }
    bool getOptimize() const { return optimize; }

private:
    size_t argc;                    ///< Argument count
    char** argv;                    ///< Arguments
    bool vizFlag = false;           ///< Flag for visualization
    bool plotFlag = false;          ///< Flag for plotting 
    std::string vizDir = "viz";     ///< Directory where PPM output is saved
    std::string plotDir = "data";   ///< Directory where plot data is saved
    int steps = 1000;               ///< Number of steps
    int width = 100;                ///< Grid width (road length)
    int height = 100;               ///< Grid height (lanes)
    int vmax = 3;                   ///< Max velocity
    double prob = 0.3;              ///< Braking probability
    double density = 0.5;           ///< Initial car density (0-1)
    bool optimize = false;          ///< Add straight lane to east inbound and west outbound if true
};

#endif // ARG_PARSER_HPP