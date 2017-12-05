#pragma once

#define SAFE_DELETE_OBJECT(x)  if((x) != NULL){ delete (x);     (x) = NULL;} // delete an object
#define SAFE_DELETE_ARRAY(x)   if((x) != NULL){ delete [] (x);  (x) = NULL;} // delete an array
#define SAFE_RELEASE(x)	       if((x) != NULL){ (x)->Release(); (x) = NULL;} // Release a COM object...
