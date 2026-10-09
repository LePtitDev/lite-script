#ifndef LITESCRIPT_PLAYGROUND_TOOLS_TOOLS_HPP
#define LITESCRIPT_PLAYGROUND_TOOLS_TOOLS_HPP

// The development tools of the playground, used by the editors
namespace Tools {

    /**
     * Run the language server (Language Server Protocol on the standard input and output)
     *
     * @return The exit code
     */
    int RunLanguageServer();

    /**
     * Run the debug adapter (Debug Adapter Protocol on the standard input and output)
     *
     * @return The exit code
     */
    int RunDebugAdapter();

}

#endif //LITESCRIPT_PLAYGROUND_TOOLS_TOOLS_HPP
