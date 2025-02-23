#include <filesystem>
#include <iostream>
#include <functional>

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;



namespace fs = std::filesystem;

using namespace std;

void CopyRecursive(const fs::path& src, const fs::path& target,
                   const std::function<bool(const fs::path&)>& predicate) noexcept
{
    try
    {
        if (!fs::exists(src) || !fs::is_directory(src))
        {
            std::cerr << "Source directory does not exist or is not a directory: " << src << '\n';
            return;
        }

        for (const auto& dirEntry : fs::recursive_directory_iterator(src))
        {
            const auto& p = dirEntry.path();
            if (predicate(p))
            {
                // Compute target path
                const auto relativeSrc = fs::relative(p, src);
                const auto targetPath = target / relativeSrc;

                // If it's a directory, create it
                if (fs::is_directory(p))
                {
                    fs::create_directories(targetPath);
                }
                else // Ensure parent directory exists before copying the file
                {
                    fs::create_directories(targetPath.parent_path());

                    // Use copy options to handle symlinks, overwrites, etc.
                    fs::copy(p, targetPath, fs::copy_options::overwrite_existing | fs::copy_options::copy_symlinks);
                }
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << " | Path1: " << e.path1() << " | Path2: " << e.path2() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "General error: " << e.what() << '\n';
    }
}

#include "inja.hpp"

// Just for convenience
using namespace inja;

int main()
{

    // ...

    fs::path appRoot = APP_ROOT_FOLDER;
    fs::path templateRoot = APP_TEMPLATE_FOLDER;;

    cout << "appRoot: " << appRoot << endl;
    cout << "templateRoot: " << templateRoot << endl;
    appRoot = appRoot.lexically_normal();
    templateRoot = templateRoot.lexically_normal();
    cout << "appRoot: " << appRoot << endl;
    cout << "templateRoot: " << templateRoot << endl;


    std::string newProjectName;

    cout << "Please enter the name of the new project (no spaces!)" << endl;

    cin >> newProjectName;

    fs::path newProjectPath = appRoot / newProjectName;

    cout << "Creating new project at " << newProjectPath << endl;

    fs::create_directories(newProjectPath);

    CopyRecursive(templateRoot, newProjectPath, [](fs::path p) -> bool
    {
        return p.extension() != ".git";
    });

    cout << "Done!" << endl;

    cout << "Note:  You will need to re-run CMake on your apps folder to pick up the new project." << endl;
    cout << "To do this, in Qt Creator, go to the 'Projects' tab, right-click on your apps folder, and select 'Run CMake'" << endl;
    cout << "The new project will then appear in your list of applications.  To make that project the one the runs when you click the green play button," << endl;
    cout << "Click the \"Projects\" icon (with the wrench) on the left, then select \"run\" under \"Build and Run\", and finally " << endl;
    cout << "select the new project from the drop-down list next to \"Run Configuration\"" << endl;

    return 0;
}

