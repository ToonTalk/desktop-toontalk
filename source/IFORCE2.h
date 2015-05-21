/*
**  File:       IForce2.h
**
**  Contains:   IForce2.DLL API
**
**				Copyright (c) 1997 Immersion Corporation
*/
#ifndef IForce2_h
#define IForce2_h

#ifndef __DINPUT_INCLUDED__
	#error include 'dinput.h' before including this file for DirectInput structures.
#endif /* !__DINPUT_INCLUDED__ */

#define IFORCEAPI __stdcall

#if !defined(_IFORCEDLL_)
#define DLLAPI __declspec(dllimport)
#else
#define DLLAPI __declspec(dllexport)
#endif

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
**  CONSTANTS
*/

/*
**  RT_IFORCE - Resource type for IFORCE projects stored as resources.
**   This is the resource type looked for by IFLoadProjectResource().
*/
#define	RT_IFORCE	((LPCSTR)"IFORCE")


/*
**  TYPES/STRUCTURES
*/

/*
**  HIFORCEPROJECT - used to identify a loaded project as a whole.
**   individual objects within a project are uniquely referenced by name.
**   Created by the IFLoadProject*() functions and released by IFReleaseProject().
*/
typedef	LPVOID HIFORCEPROJECT;

/*
**  IForceEffect - contains the information needed to create a DI effect
**   using IDirectInputEffect::CreateEffect(). An array of pointers to these
**	 structures is allocated and returned by IFCreateEffectStructs().
*/
typedef struct {
	GUID		guid;
	LPDIEFFECT	lpDIEffect;
} IForceEffect;


/*
**  FUNCTION DECLARATIONS
*/

/*
**  IFLoadProjectResource() - Load a project from a resource.
**   hRsrcModule - handle of the module containing the project definition resource.
**   pRsrcName - name or MAKEINTRESOURCE(id) identifier of resource to load.
**   pDevice - device for which the project is being loaded. If NULL,
**     effects will be created generically, and IFCreateEffects() will fail.
**   Returns an identifier for the loaded project, or NULL if unsuccessful.
*/
DLLAPI
HIFORCEPROJECT
IFORCEAPI
IFLoadProjectResource(
	HMODULE hRsrcModule,
	LPCSTR pRsrcName,
	LPDIRECTINPUTDEVICE2A pDevice );

/*
**  IFLoadProjectPointer() - Load a project from a pointer.
**   pProject - points to a project definition.
**   pDevice - device for which the project is being loaded. If NULL,
**     effects will be created generically, and IFCreateEffects() will fail.
**   Returns an identifier for the loaded project, or NULL if unsuccessful.
*/
DLLAPI
HIFORCEPROJECT
IFORCEAPI
IFLoadProjectPointer(
	LPVOID pProject,
	LPDIRECTINPUTDEVICE2A pDevice );

/*
**  IFLoadProjectFile() - Load a project from a file.
**    pProjectFileName - points to a project file name.
**    pDevice - device for which the project is being loaded. If NULL,
**       effects will be created generically, and IFCreateEffects() will fail.
**    Returns an identifier for the loaded project, or NULL if unsuccessful.
*/
DLLAPI
HIFORCEPROJECT
IFORCEAPI
IFLoadProjectFile(
	LPCSTR pProjectFileName,
	LPDIRECTINPUTDEVICE2A pDevice );

/*
**  IFLoadProjectObjectPointer() - Load a project from a pointer to a single
**     object definition (usually used only by the editor).
**   pObject - points to an object definition.
**   pDevice - device for which the project is being loaded. If NULL,
**     effects will be created generically, and IFCreateEffects() will fail.
**   Returns an identifier for the loaded project, or NULL if unsuccessful.
*/
DLLAPI
HIFORCEPROJECT
IFORCEAPI
IFLoadProjectObjectPointer(
	LPVOID pObject,
	LPDIRECTINPUTDEVICE2A pDevice );

/*
**  IFReleaseProject() - Release a loaded project.
**   hProject - identifies the project to be released.
**   Returns TRUE if the project is released, FALSE if it is an invalid project.
*/
DLLAPI
BOOL
IFORCEAPI
IFReleaseProject(
	HIFORCEPROJECT hProject );

/*
**  IFCreateEffectStructs() - Create IForceEffects for a named effect.
**   hProject - identifies the project containing the object.
**   pObjectName - name of the object for which to create structures.
**   pNumEffects - if not NULL will be set to a count of the IForceEffect
**     structures in the array (not including the terminating NULL pointer.)
**   Returns a pointer to the allocated array of pointers to IForceEffect
**     structures. The array is terminated with a NULL pointer. If the
**     function fails, a NULL pointer is returned.
*/
DLLAPI
IForceEffect **
IFORCEAPI
IFCreateEffectStructs(
	HIFORCEPROJECT hProject,
	LPCSTR pObjectName,
	int *pNumEffects );

/*
**  IFReleaseEffectStructs() - Release an array of IForceEffects.
**   hProject - identifies the project for which the effects were created.
**   pEffects - points to the array of IForceEffect pointers to be released.
**   Returns TRUE if the array is released, FALSE if it is an invalid array.
*/
DLLAPI
BOOL
IFORCEAPI
IFReleaseEffectStructs(
	HIFORCEPROJECT hProject,
	IForceEffect **pEffects );

/*
**  IFCreateEffects() - Creates the DirectInput effects using
**     IDirectInput::CreateEffect().
**   hProject - identifies the project containing the object.
**   pObjectName - name of the object for which to create effects.
**   pNumEffects - if not NULL will be set to a count of the IDirectInputEffect
**     pointers in the array (not including the terminating NULL pointer.)
**   Returns a pointer to the allocated array of pointers to IDirectInputEffects.
**     The array is terminated with a NULL pointer. If the function fails,
**     a NULL pointer is returned.
*/
DLLAPI
LPDIRECTINPUTEFFECT *
IFORCEAPI
IFCreateEffects(
	HIFORCEPROJECT hProject,
	LPCSTR pObjectName,
	int *pNumEffects );

/*
**  IFReleaseEffects() - Releases an array of IDirectInputEffect structures.
**   hProject - identifies the project for which the effects were created.
**   pEffects - points to the array if IDirectInputEffect pointers to be released.
**   Returns TRUE if the array is released, FALSE if it is an invalid array.
*/
DLLAPI
BOOL
IFORCEAPI
IFReleaseEffects(
	HIFORCEPROJECT hProject,
	LPDIRECTINPUTEFFECT	*pEffects );


#if defined __cplusplus
}
#endif /* __cplusplus */

#endif /* !IForce2_h */
