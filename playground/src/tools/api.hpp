#ifndef LITESCRIPT_PLAYGROUND_TOOLS_API_HPP
#define LITESCRIPT_PLAYGROUND_TOOLS_API_HPP

#include <string_view>
#include <vector>

namespace Tools {

    // An element of the libraries available in the playground (standard library and playground modules)
    struct ApiEntry {

        enum class Kind {
            // A global function or a function of a namespace
            Function,
            // A namespace of a library
            Namespace,
            // A value (constant or property)
            Value,
            // A method of the arrays
            ArrayMethod,
            // A method of the strings
            StringMethod
        };

        Kind kind;
        // The name: "print", "Math", "Math.abs", "push" (methods have no prefix)
        std::string_view name;
        // The signature: "abs(x)"
        std::string_view signature;
        // The description (Markdown)
        std::string_view documentation;

    };

    /**
     * Return every element of the libraries
     */
    const std::vector<ApiEntry>& GetApi();

    /**
     * Find an element by name ("Math.abs", "print"), nullptr if it doesn't exist
     */
    const ApiEntry * FindApi(std::string_view name);

    // The keywords of the language
    extern const std::vector<std::string_view> KEYWORDS;

}

#endif //LITESCRIPT_PLAYGROUND_TOOLS_API_HPP
