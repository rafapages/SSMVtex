# Seamless, static, 3D mesh multi-texturer

This project multi-textures a 3D plain (un-textured) mesh using a set of images of the model. The result is a texture atlas which blends the color information from the different images seamlessly. This system also supports a color-per-vertex coloring approach, which does not produce a texture atlas, but seamlessly colors a dense triangle mesh.

Further details can be found in the following paper:

[Seamless, Static Multi-Texturing of 3D Meshes] (http://onlinelibrary.wiley.com/doi/10.1111/cgf.12508/abstract)

(Computer Graphics Forum, Feb. 2015)

## Build

Navigate to the directory and hit *make*. 

## Dependencies

- FreeImagePlus: to read/write images.
- OpenCV: to find a face in a given image and improve the texture quality of that particular area.
- Eigen3: for the linear algebra stuff, mainly.

## Usage
 
	multitex [options] <fileNameIn> <fileNameCam> <fileNameImageList> [<fileNameOut> <fileNameTexOut>]

* _fileNameIn_: The name of the input text file containing the 3D scene described in OBJ format. Extension : obj.
* _fileNameCam_: The name of the input text file containing the camera calibration matrices. Extension: txt or out.
* _fileNameImageList_: The name of the input text file containing the list of images that will be used for texturing the mesh. Extension: txt.
* _fileNameOut_: The name of the output text file that'll be created containing the textured model -- if not specified, it'll be built by appending _options_ to _fileNameIn_. Extensions: wrk, obj or ply.
* _fileNameTexOut_: The name of the output customized texture that'll be created, -- if not specified, it'll be built by appending _options_ to _fileNameIn_. Extension: jpg.

##### Options:
* -{n|b|a|l|p}	Assing cameras to triangles using (n) their normals, (b) their normals using the barycenter technique, (a) their area,(l)	or their area taking occlusions into account, (p) or both occlusions and checking photoconsistency. Default: p.
* -{m|s}	 Input value is (m) common 3D mesh or (s) a splat based 3D mesh. Default: m
* -{#}		Number of maximum images mixed per triangle. Default: 2 (not very smooth mixing).
* -{v|t|f} Show (v) a mesh colored per vertex, (t) a mesh with textures or (f) a mesh colored with a flat color per chart. Default: t.
* -o highlights occlusions in yellow.
* —faceCam=_imageFileName-   in case there a frontal image showing the subject's face.
* —alpha=_alpha_ is the cutoff value of the normal weighting function, in the interval (0, 1). Default: 0.5.
* —beta=_beta_ is the curvature of the normal weighting function, in the interval [0, inf). Default: 1.
* —dimension=_dimension_ is the resolution of the output image measured in Mpixels. Default: 1.
* —width=_width_ is width of the output image measured in pixels. If this value is greater than zero, then _dimension_ is ignored.
* —cache=_cachesize_ size of the image cache. Default: 75.
* -h		Prints help message.


## About the camera calibration file

The camera calibration file that needs to be provided follows the scheme below:

	[ Intrinsic parameters (row1 row2 row3) ] [ Rotation matrix (row1 row2 row3) ] [ Camera position ] [ Image dimensions ] [ (Optional) Radial distortion coefficients (k1 k2)]

Radial distortion coefficients are optional by now, as we are not correcting this distortion yet. The model used consideres the camera to be looking towards +z axis. You can also provided a [Bundler-SfM] (https://github.com/snavely/bundler_sfm) file, but only the camera information will be read.

## About the photoconsistency check

As many 3D models have been acquired using a SfM approach, it is possible that people or objects occlude partially the reconstructed scene. If these occlusions are not handled correctly, there might be included in the texture: a person walking in front of the scene is the most typical case. We have added a photoconsistency check function which discards the color information from occluding subjects. To do so, we perform a statistical analysis of the color provided by different cameras.  However, when we deal with very controlled scenarios, such as models reconstructed using a Kinect fusion based approach, or Shape from Silhouette, there might not be enough images to perform this statistical analysis correctly. In this last case, we recommend using the flag ‘-l’, which turns the photoconsistency check off.

## Citation

If you use this software for scientific purposes, please don’t forget to cite us! Thanks!

	@article {pages2015seamless,
	author = {Pag\'{e}s, R. and Berj\'{o}n, D. and Mor\'{a}n, F. and Garc\'{i}a, N.},
	title = {Seamless, Static Multi-Texturing of {3D} Meshes},
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



