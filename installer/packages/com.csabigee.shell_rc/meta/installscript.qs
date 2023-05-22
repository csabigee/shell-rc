function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    // This actually installs the files
    component.createOperations();

    if (systemInfo.productType == "windows") {
        // Start menu shortcut
        component.addOperation("CreateShortcut", 
                               "@TargetDir@/shell-rc.exe", 
                               "@StartMenuDir@/shell-rc.lnk", 
                               "workingDirectory=@TargetDir@", 
                               "iconPath=@TargetDir@/shell_rc.ico");

       // Desktop Shortcut
       component.addOperation("CreateShortcut", 
                              "@TargetDir@/shell-rc.exe", 
                              "@DesktopDir@/shell_rc.lnk",
                              "workingDirectory=@TargetDir@", 
                              "iconPath=@TargetDir@/shell_rc.ico");
    }
}