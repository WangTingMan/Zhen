#include "ModuleManager.h"

#define MODULE_MANAGER_NAME "module_manager"

ModuleManager::ModuleManager()
{
    SetModuleName( MODULE_MANAGER_NAME );
}

int ModuleManager::Init()
{
    for( auto it = m_modules.begin(); it != m_modules.end(); ++it )
    {
        it->second->Init();
    }
    return 0;
}

void ModuleManager::Stop()
{
    for( auto it = m_modules.begin(); it != m_modules.end(); ++it )
    {
        it->second->Stop();
    }
}

void ModuleManager::Release()
{
    for( auto it = m_modules.begin(); it != m_modules.end(); ++it )
    {
        it->second->Release();
    }
}

std::shared_ptr<AbstractModule> ModuleManager::GetMoudle( std::string const& a_name )
{
    std::shared_ptr<AbstractModule> ret;
    auto it = m_modules.find( a_name );
    if( it != m_modules.end() )
    {
        ret = it->second;
    }
    return ret;
}

bool ModuleManager::AddNewModule( std::shared_ptr<AbstractModule> a_module )
{
    m_modules.insert_or_assign( a_module->GetModuleName(), a_module );
    return true;
}

