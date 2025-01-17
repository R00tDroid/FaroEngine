#pragma once
#include "Toolchain.hpp"
#include "Configuration.hpp"

struct ConfigurationMSVC : Configuration
{
    
};

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();

    unsigned configurations() override;
    Configuration* configuration(unsigned index) override;
};
