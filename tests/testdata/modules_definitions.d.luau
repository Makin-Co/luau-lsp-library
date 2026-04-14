--#METADATA#{"MODULES":["ModuleA","ModuleB"],"MODULES_TYPE":"Library","MODULES_METHOD":"GetModule"}

declare class ModuleA
    function doSomething(self): string
end

declare class ModuleB
    function doOtherThing(self): number
end

declare class Library
    function GetModule(self, name: string): Instance
end

declare Library: Library
