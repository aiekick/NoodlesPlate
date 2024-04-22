// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <ctools/ConfigAbstract.h>
#include <string>

class ProjectFile : public conf::ConfigAbstract {
public:  // to save
    std::string puProjectFilePathName;
    std::string puProjectFilePath;

public:   // dont save
private:  // dont save
    bool puIsLoaded = false;
    bool puNeverSaved = false;
    bool puIsThereAnyNotSavedChanged = false;

public:
    ProjectFile();
    ~ProjectFile();

    void Clear();
    void New();
    void New(const std::string& vFilePathName);
    bool Load();
    bool LoadAs(const std::string vFilePathName);  // ils wanted to not pass the adress for re open case
    // void LoadFile(const std::string& vFilePathName);
    bool Save();
    bool SaveAs(const std::string& vFilePathName);
    bool IsLoaded() const;

    bool IsThereAnyNotSavedChanged() const;
    void SetProjectChange(bool vChange = true);

    std::string GetAbsolutePath(const std::string& vFilePathName) const;
    std::string GetRelativePath(const std::string& vFilePathName) const;

public:
    static ProjectFile* Instance() {
        static ProjectFile _instance;
        return &_instance;
    }

public:
    std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;
};
