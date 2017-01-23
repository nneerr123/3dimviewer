///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2016 3Dim Laboratory s.r.o.
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

#include <osg/osgcompat.h>

#if OSG_VERSION_GREATER_OR_EQUAL(3,1,10)
	UniDrawableList getGeodeDrawableList(osg::Geode *pGeode)
	{
		UniDrawableList list;
		if (NULL!=pGeode)
		{
			unsigned int nDrawables = pGeode->getNumDrawables();
			for(unsigned int i = 0; i < nDrawables; i++)
				list.push_back(pGeode->getDrawable(i));
		}
		return list;
	}
#else
	UniDrawableList getGeodeDrawableList(osg::Geode *pGeode)
	{
		if (NULL==pGeode)
		{
			UniDrawableList list;
			return list;
		}
		return pGeode->getDrawableList();
	}
#endif