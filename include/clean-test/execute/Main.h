// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

namespace clean_test::execute {

class Configuration;

/// The default implementation of an execution frontend for users.
///
/// Honors various commandline parameters as specified via @p argc and @p argv.
int main(int argc, char ** argv);

/// Execute framework as specified by @p configuration.
int main(Configuration const & configuration);

}
