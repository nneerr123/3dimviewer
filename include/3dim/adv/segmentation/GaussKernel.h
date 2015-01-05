///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2012 3Dim Laboratory s.r.o.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef LEVELSET_GAUSS
#define LEVELSET_GAUSS

/*
* Class representing gauss kernel and operations
* above the kernell.
*/
class CGaussKernel
{
public:
	double ***kernel_3D;
	
	int size;
	int size_half;

public:	
	CGaussKernel();	
	~CGaussKernel();
	//computes the kernel
	void computeKernel(int size, double sigma);
};

#endif