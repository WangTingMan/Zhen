#include <iostream>
#include <chrono>
#include <Zhen/KeyboardReader.h>

#include "HomePage.h"
#include "Zhen/logging.h"
#include "Zhen/PageManager.h"

int main()
{
    LogDebug() << "Start to run.";
    KeyboardReader reader;
    reader.start();

    std::shared_ptr<BasePage> page = std::make_shared<HomePage>();
    PageManager::GetInstance().PushPage( std::move( page ) );
    PageManager::GetInstance().SetDisplayAreaWidth( 120 );
    PageManager::GetInstance().run();
    reader.stop();
    return 0;
}
