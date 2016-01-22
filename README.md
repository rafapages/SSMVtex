# Seamless, static, 3D mesh multi-texturer

This project multi-textures a 3D plain (un-textured) mesh using a set of images of the model. The result is a texture atlas which blends the color information from the different images seamlessly.

Further details can be found in the following paper:

[Seamless, Static Multi-Texturing of 3D Meshes] (http://onlinelibrary.wiley.com/doi/10.1111/cgf.12508/abstract)

## Build

To fill in yet…

## Usage
 
	multitex [options] <fileNameIn> <fileNameCam> <fileNameImageList> [<fileNameOut> <fileNameTexOut>]

* _fileNameIn_: The name of the input text file containing the 3D scene described in OBJ format. Extension : obj.
* _fileNameCam_: The name of the input text file containing the camera calibration matrices. Extension: txt.
* _fileNameImageList_: The name of the input text file containing the list of images that will be used for texturing the mesh. Extension: txt.
* _fileNameOut_: The name of the output text file that'll be created containing the textured model -- if not specified, it'll be built by appending "options" to <fileNameIn>. Extensions: vrml, obj or ply.
* _fileNameTexOut_: The name of the output customized texture that'll be created, -- if not specified, it'll be built by appending "options" to <fileNameIn>. Extension: jpg.

- Options:
* -{n|b|a|l}	Assing cameras to triangles using (n) their normals, (b) their normals using the baricenter technique, (a) their area,(l)	or their area taking occlusions into account. Default: l.
* -{m|s}	Input value is (m) common 3D mesh or (s) a splat based 3D mesh. Default: m
* -{1-9}	If B option has been chosen, this argument indicates the maximum amount of images per triangle used in order to create	the customized texture. Default: 1.
* -{e|u|p|t} Show (e) a naked mesh, (u) a mesh colored per triangle, (p) a mesh colored per vertex or (t) a mesh with textures in the VRML97 file. Default: t.
* —faceCam=<imageFileName>   in case there a frontal image showing the subject's face.
* —alpha=_alpha_ alpha is the cutoff value of the normal weighting function, in the interval (0, 1). Default: 0.5.
* —beta=_beta_ beta is the curvature of the normal weighting function, in the interval [0, inf). Default: 1.
* —dimension=_dimension_ resolution of the output image measured in Mpixels.
* —cache=_cachesize_ size of the image cache. Default: 75.
* -h		Prints help message.




## Citation

If you use this software for scientific purposes, please don’t forget to cite us! Thanks!

	@article {pages2015seamless,
	author = {Pagés, R. and Berjón, D. and Morán, F. and García, N.},
	title = {Seamless, Static Multi-Texturing of 3D Meshes},
	journal = {Computer Graphics Forum},
	volume = {34},
	number = {1},
	issn = {1467-8659},
	url = {http://dx.doi.org/10.1111/cgf.12508},
	doi = {10.1111/cgf.12508},
	pages = {228--238},
	keywords = {texture mapping, texture synthesis
	ACM CCS: I.3.7 [Computer Graphics]: Three-Dimensional Graphics and Realism—Colour, shading, shadowing and texture},
	year = {2015},
	}
