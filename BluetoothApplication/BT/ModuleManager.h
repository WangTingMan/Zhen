#pragma once
#include "AbstractModule.h"

#include <string>
#include <unordered_map>
#include <memory>

class ModuleManager : public AbstractModule
{

public:

    ModuleManager();

    int Init()override;

    void Stop()override;

    void Release()override;

    std::shared_ptr<AbstractModule> GetMoudle( std::string const& a_name );

    bool AddNewModule( std::shared_ptr<AbstractModule> a_module );

private:

    std::unordered_map<std::string, std::shared_ptr<AbstractModule>> m_modules;

};

