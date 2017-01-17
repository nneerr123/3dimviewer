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

#ifndef MIP_H
#define MIP_H

namespace shader
{

const char MAX_IP[] =
"#ifndef __GLSL_CG_DATA_TYPES \n"
"#define saturate(type) clamp(type,0.0,1.0) \n"
"#endif \n"
" \n"
"uniform sampler3D t3D;                  // 3D texture containing data \n"
"uniform sampler2D LookUp;               // color and transparency look-up texture \n"
"uniform sampler3D tRaysStartEnd;        // back-facing world coords \n"
"uniform sampler2D Noise;                // Noise offset used to remove aliasing [replacable by internal GLSL noise generator] \n"
"uniform sampler2D Depth; \n"
" \n"
"varying vec4 fragWorldPosition; \n"
" \n"
"uniform vec2 inputAdjustment; \n"
"uniform vec2 imageAdjustment; \n"
"uniform vec3 tResolution; \n"
"uniform vec2 wSize;             // Viewport size 1/X and 1/Y \n"
"uniform vec4 pl;                // plane that cuts the volume \n"
"uniform float textureSampling;  // 3D texture sampling rate \n"
"uniform mat4 invProjectionMatrix; \n"
"uniform mat4 invModelViewMatrix; \n"
" \n"
"uniform sampler3D tSkip3D;      // skipping 3D texture \n"
"uniform vec3 skipTexSize; \n"
"uniform vec3 tSkipResolution; \n"
" \n"
"uniform vec4 skipCondition; \n"
" \n"
"// Compute position against the cutting plane \n"
"float cuttingPlane(vec3 pos) \n"
"{ \n"
"    return (pl.x * pos.x + pl.y * pos.y + pl.z * pos.z + pl.w); \n"
"} \n"
" \n"
"// Compute plane intersection \n"
"float planeCut(vec3 pos, float value, float defvalue) \n"
"{ \n"
"    return (cuttingPlane(pos) > 0.0 ? value : defvalue); \n"
"} \n"
" \n"
"// Use the skipping volume to evaluate the given position \n"
"int isMeaningful(vec3 pos) \n"
"{ \n"
"    // is the voxel otside of the cutting plane \n"
"    if (cuttingPlane(pos) < 0.0) \n"
"    { \n"
"        return 0; \n"
"    } \n"
" \n"
"    // sample a voxel at initial position \n"
"    vec4 voxel = texture3D(tSkip3D, pos * skipTexSize); \n"
" \n"
"    // minimum and maximum voxel value  \n"
"    float vmin = voxel.r; \n"
"    float vmax = voxel.g; \n"
"    vmin = saturate((vmin * inputAdjustment.x + 0.001) + inputAdjustment.y); \n"
"    vmax = saturate((vmax * inputAdjustment.x + 0.001) + inputAdjustment.y); \n"
" \n"
"    // is the position considered empty? \n"
"    return (vmin > skipCondition.y || vmax < skipCondition.x) ? 0 : 1; \n"
"} \n"
" \n"
"// Compute length of the step in the 3D texture \n"
"float stepLength(vec3 path) \n"
"{ \n"
"    // All converted to the first quadrant \n"
"    vec3 aPath = abs(path); \n"
" \n"
"    // to be explained later \n"
"    // basically computes line/plane intersection \n"
"    vec3 t = tResolution.xyz / (aPath + 1e-20); \n"
"    float d2x = length(aPath*t.x); // intersection with a plane perpendicular to the X axis \n"
"    float d2y = length(aPath*t.y); // intersection with a plane perpendicular to the Y axis \n"
"    float d2z = length(aPath*t.z); // intersection with a plane perpendicular to the Z axis \n"
" \n"
"    // return the closest intersection - that is, the correct one \n"
"    return min(d2x, min(d2y, d2z)); \n"
"} \n"
" \n"
"// Compute ray depth in depth buffer \n"
"float computeDepth(vec3 backWC, vec3 frontWC, float rayTermination) \n"
"{ \n"
"    vec3 terminationWC = frontWC + ((backWC - frontWC) * vec3(rayTermination)); \n"
"    vec4 iproj = gl_ModelViewProjectionMatrix * vec4(terminationWC, 1); \n"
"    iproj.z /= iproj.w; \n"
"    return (iproj.z + 1.0) * 0.5; \n"
"} \n"
" \n"
" \n"
"void main() \n"
"{ \n"
"    // sample ray's start and end points \n"
"    vec4 backPPos = vec4(texture3D(tRaysStartEnd, vec3(gl_FragCoord.xy * wSize, 1.5 / 2.0)).xyz, 1.0); \n"
"    vec4 frontPPos = vec4(backPPos.xy, texture3D(tRaysStartEnd, vec3(gl_FragCoord.xy * wSize, 0.5 / 2.0)).z, 1.0); \n"
"    vec4 backVPos = invProjectionMatrix * backPPos; \n"
"    vec4 frontVPos = invProjectionMatrix * frontPPos; \n"
"    frontVPos /= frontVPos.w; \n"
"    backVPos /= backVPos.w; \n"
"    vec4 frontWPos = invModelViewMatrix * frontVPos; \n"
"    vec4 backWPos = invModelViewMatrix * backVPos; \n"
" \n"
"    vec3 frontWC = frontWPos.xyz; \n"
"    vec3 backWC =  backWPos.xyz; \n"
"    vec4 frontTC = vec4(frontWC * 0.5 + 0.5, 0.0); \n"
"    vec4 backTC =  vec4(backWC * 0.5 + 0.5, 0.0); \n"
" \n"
"    // set initial fragment color/value \n"
"    vec3 texel = vec3(0.0); \n"
" \n"
"    // set intilial monocromatic color \n"
"    float oldMonochromatic = 0.0; \n"
" \n"
"    // set ray direction \n"
"    vec3 RayPath = backTC.rgb - frontTC.rgb; \n"
" \n"
"    // normalized direction \n"
"    vec3 nPath = normalize(RayPath); \n"
" \n"
"    // adjust length of the step (vector)   \n"
"    vec3 CoarsePath = nPath * tSkipResolution.x; \n"
" \n"
"    // adjust length of the step (vector)   \n"
"    //    vec3 FinePath = nPath * stepLength(RayPath); \n"
"    vec3 FinePath = nPath * tResolution.x; \n"
" \n"
"    // distance to travel by the ray \n"
"    float tDistance = length(RayPath); \n"
" \n"
"    // length of a coarse step (scalar) \n"
"    float tCoarseStep = length(CoarsePath); \n"
" \n"
"    // length of a fine step (scalar) \n"
"    float tFineStep = length(FinePath); \n"
" \n"
"    // calculate the number of fine steps in a single coarse step \n"
"    int NumOfFineSteps = int(tCoarseStep / tFineStep); \n"
" \n"
"    // where to start in the texture (scalar) \n"
"    float tPositionStart = texture2D(Noise, (gl_FragCoord.xy - vec2(0.5)) * wSize).r * tFineStep; \n"
" \n"
"    // where to start in the texture (vector) \n"
"    vec3 vtPositionStart = frontTC.rgb + nPath * tPositionStart; \n"
" \n"
"    // is the initial voxel meaningful? \n"
"    int voxelFlagStart = isMeaningful(vtPositionStart); \n"
" \n"
"    // outer loop \n"
"    for (int i = 0; i < 256; i++) \n"
"    { \n"
"        // check if the ray is still vithin the textured volume \n"
"        if (tPositionStart > tDistance) \n"
"            break; \n"
" \n"
"        // next position \n"
"        vec3 vtPositionEnd = vtPositionStart + CoarsePath; \n"
"        float tPositionEnd = tPositionStart + tCoarseStep; \n"
" \n"
"        // is the end position meaningful? \n"
"        int voxelFlagEnd = isMeaningful(vtPositionEnd); \n"
" \n"
"        // should this part of the volume be passed \n"
"        if ((voxelFlagStart + voxelFlagEnd) != 0) \n"
"        { \n"
"            // inner loop \n"
"            vec3 vtPosition = vtPositionStart; \n"
"            float tPosition = tPositionStart; \n"
"            for (int j = 0; j < NumOfFineSteps; j++) \n"
"            { \n"
"                // z-buffer check \n"
"                if (texture2D(Depth, (gl_FragCoord.xy) * wSize).r < computeDepth(backWC, frontWC, tPosition / tDistance)) \n"
"                    break; \n"
" \n"
"                // sample voxel at the current position \n"
"                float voxel = planeCut(vtPosition, saturate((texture3D(t3D, vtPosition).r * inputAdjustment.x + 0.001) + inputAdjustment.y), 0.0); \n"
" \n"
"                // get color value from look-up table \n"
"                vec4 LUT = texture2D(LookUp, vec2(voxel, 0.5)); \n"
" \n"
"                // estimate monochromatic intensity and compare maximum \n"
"                float newMonochromatic = 0.2989 * LUT.r + 0.5870 * LUT.g + 0.1140 * LUT.b; \n"
" \n"
"                // checks if the current texel is a new maximum \n"
"                if (newMonochromatic > oldMonochromatic) \n"
"                { \n"
"                    texel = LUT.rgb; \n"
"                    oldMonochromatic = newMonochromatic; \n"
"                } \n"
" \n"
"                // move ray to the next position - both the scalar and the vector \n"
"                vtPosition += FinePath; \n"
"                tPosition += tFineStep; \n"
"            } \n"
"        } \n"
" \n"
"        // move to the next position \n"
"        vtPositionStart = vtPositionEnd; \n"
"        tPositionStart = tPositionEnd; \n"
"        voxelFlagStart = voxelFlagEnd; \n"
"    } \n"
" \n"
"    vec4 outputColor = vec4(saturate((texel * imageAdjustment.y) + imageAdjustment.x), 1.0); \n"
"    float alpha = outputColor.r * 3.0; \n"
"    gl_FragData[0] = outputColor; \n"
"    gl_FragData[1] = vec4(0.0, alpha, 0.0, 0.0); \n"
"} \n";

} // namespace shader

#endif // MIP_H

