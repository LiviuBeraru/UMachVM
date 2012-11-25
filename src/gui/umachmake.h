#ifndef UMACHMAKE_H
#define UMACHMAKE_H

class Project;
class UMachMake
{
public:
    UMachMake();

    static bool makeUMXfromProject(Project *project);
};

#endif // UMACHMAKE_H
