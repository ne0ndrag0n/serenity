/*
 * Copyright (c) 2021-2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/LexicalPath.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/Stream.h>
#include <LibCpp/Preprocessor.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    Core::ArgsParser args_parser;
    StringView path;
    bool print_definitions = false;
    args_parser.add_positional_argument(path, "File", "file", Core::ArgsParser::Required::Yes);
    args_parser.add_option(print_definitions, "Print preprocessor definitions", "definitions", 'D');
    args_parser.parse(arguments);

    auto file = TRY(Core::Stream::File::open(path, Core::Stream::OpenMode::Read));
    auto content = TRY(file->read_all());
    String name = LexicalPath::basename(path);
    Cpp::Preprocessor cpp(name, StringView { content });
    auto tokens = cpp.process_and_lex();

    if (print_definitions) {
        outln("Definitions:");
        for (auto const& definition : cpp.definitions()) {
            if (definition.value.parameters.is_empty())
                outln("{}: {}", definition.key, definition.value.value);
            else
                outln("{}({}): {}", definition.key, String::join(',', definition.value.parameters), definition.value.value);
        }
        outln("");
    }

    for (auto& token : tokens) {
        outln("{}", token.to_string());
    }

    return 0;
}
