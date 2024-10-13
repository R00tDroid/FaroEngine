#pragma once
#include "FaroProjectsExports.generated.h"

class FaroProjectsExports IManifest
{
public:
    IManifest(const char* manifestLocation);
    virtual ~IManifest();

    // Path to the manifest file
    const char* manifestPath() const;

    // Path to the directory containing the manifest
    const char* manifestDirectory() const;

    // Path to the hidden .Faro directory in which to generate intermediate files
    const char* faroDirectory() const;

    // Path to the hidden .Faro/Info directory in which to generate information cache files
    const char* cacheDirectory() const;

private:
    struct Impl;
    Impl* impl = nullptr;
};
