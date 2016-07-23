xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 496;
 -0.66291;0.00000;0.00000;,
 0.00000;0.66291;0.00000;,
 0.00000;0.00000;-0.62500;,
 0.00000;-0.66291;-0.00000;,
 0.66291;0.00000;0.00000;,
 0.00000;0.00000;0.62500;,
 0.00000;-0.66291;-0.00000;,
 -0.66291;0.00000;0.00000;,
 0.00000;0.00000;-0.62500;,
 0.66291;0.00000;0.00000;,
 0.00000;0.66291;0.00000;,
 0.00000;0.00000;0.62500;,
 0.00000;0.66291;0.00000;,
 0.66291;0.00000;0.00000;,
 0.00000;0.00000;-0.62500;,
 -0.66291;0.00000;0.00000;,
 0.00000;-0.66291;-0.00000;,
 0.00000;0.00000;0.62500;,
 0.66291;0.00000;0.00000;,
 0.00000;-0.66291;-0.00000;,
 0.00000;0.00000;-0.62500;,
 0.00000;0.66291;0.00000;,
 -0.66291;0.00000;0.00000;,
 0.00000;0.00000;0.62500;,
 -0.75000;0.75000;1.00000;,
 -0.75000;1.00000;1.00000;,
 -1.00000;1.00000;1.00000;,
 -1.00000;0.75000;1.00000;,
 0.75000;0.75000;1.00000;,
 0.75000;1.00000;1.00000;,
 1.00000;0.75000;1.00000;,
 1.00000;1.00000;1.00000;,
 -0.75000;-0.75000;1.00000;,
 -1.00000;-0.75000;1.00000;,
 1.00000;-0.75000;1.00000;,
 0.75000;-0.75000;1.00000;,
 -0.75000;-1.00000;1.00000;,
 -1.00000;-1.00000;1.00000;,
 0.75000;-1.00000;1.00000;,
 1.00000;-1.00000;1.00000;,
 1.00000;0.75000;0.75000;,
 1.00000;1.00000;0.75000;,
 1.00000;1.00000;1.00000;,
 1.00000;0.75000;1.00000;,
 1.00000;0.75000;-0.75000;,
 1.00000;1.00000;-0.75000;,
 1.00000;0.75000;-1.00000;,
 1.00000;1.00000;-1.00000;,
 1.00000;-0.75000;0.75000;,
 1.00000;-0.75000;1.00000;,
 1.00000;-0.75000;-1.00000;,
 1.00000;-0.75000;-0.75000;,
 1.00000;-1.00000;0.75000;,
 1.00000;-1.00000;1.00000;,
 1.00000;-1.00000;-0.75000;,
 1.00000;-1.00000;-1.00000;,
 0.75000;0.75000;-1.00000;,
 0.75000;1.00000;-1.00000;,
 1.00000;1.00000;-1.00000;,
 1.00000;0.75000;-1.00000;,
 -0.75000;0.75000;-1.00000;,
 -0.75000;1.00000;-1.00000;,
 -1.00000;0.75000;-1.00000;,
 -1.00000;1.00000;-1.00000;,
 0.75000;-0.75000;-1.00000;,
 1.00000;-0.75000;-1.00000;,
 -1.00000;-0.75000;-1.00000;,
 -0.75000;-0.75000;-1.00000;,
 0.75000;-1.00000;-1.00000;,
 1.00000;-1.00000;-1.00000;,
 -0.75000;-1.00000;-1.00000;,
 -1.00000;-1.00000;-1.00000;,
 -1.00000;0.75000;-0.75000;,
 -1.00000;1.00000;-0.75000;,
 -1.00000;1.00000;-1.00000;,
 -1.00000;0.75000;-1.00000;,
 -1.00000;0.75000;0.75000;,
 -1.00000;1.00000;0.75000;,
 -1.00000;0.75000;1.00000;,
 -1.00000;1.00000;1.00000;,
 -1.00000;-0.75000;-0.75000;,
 -1.00000;-0.75000;-1.00000;,
 -1.00000;-0.75000;1.00000;,
 -1.00000;-0.75000;0.75000;,
 -1.00000;-1.00000;-0.75000;,
 -1.00000;-1.00000;-1.00000;,
 -1.00000;-1.00000;0.75000;,
 -1.00000;-1.00000;1.00000;,
 -0.75000;1.00000;-0.75000;,
 -0.75000;1.00000;-1.00000;,
 -1.00000;1.00000;-1.00000;,
 -1.00000;1.00000;-0.75000;,
 0.75000;1.00000;-0.75000;,
 0.75000;1.00000;-1.00000;,
 1.00000;1.00000;-0.75000;,
 1.00000;1.00000;-1.00000;,
 -0.75000;1.00000;0.75000;,
 -1.00000;1.00000;0.75000;,
 1.00000;1.00000;0.75000;,
 0.75000;1.00000;0.75000;,
 -0.75000;1.00000;1.00000;,
 -1.00000;1.00000;1.00000;,
 0.75000;1.00000;1.00000;,
 1.00000;1.00000;1.00000;,
 -0.75000;-1.00000;-1.00000;,
 -0.75000;-1.00000;-0.75000;,
 -1.00000;-1.00000;-0.75000;,
 -1.00000;-1.00000;-1.00000;,
 0.75000;-1.00000;-1.00000;,
 0.75000;-1.00000;-0.75000;,
 1.00000;-1.00000;-1.00000;,
 1.00000;-1.00000;-0.75000;,
 -0.75000;-1.00000;0.75000;,
 -1.00000;-1.00000;0.75000;,
 1.00000;-1.00000;0.75000;,
 0.75000;-1.00000;0.75000;,
 -0.75000;-1.00000;1.00000;,
 -1.00000;-1.00000;1.00000;,
 0.75000;-1.00000;1.00000;,
 1.00000;-1.00000;1.00000;,
 0.75000;0.75000;-0.75000;,
 0.75000;0.75000;0.75000;,
 0.75000;1.00000;0.75000;,
 0.75000;1.00000;-0.75000;,
 0.75000;-0.75000;0.75000;,
 0.75000;-0.75000;1.00000;,
 0.75000;0.75000;1.00000;,
 0.75000;-0.75000;-1.00000;,
 0.75000;-0.75000;-0.75000;,
 0.75000;0.75000;-1.00000;,
 0.75000;-1.00000;-0.75000;,
 0.75000;-1.00000;0.75000;,
 -0.75000;0.75000;-0.75000;,
 0.75000;0.75000;-0.75000;,
 0.75000;1.00000;-0.75000;,
 -0.75000;1.00000;-0.75000;,
 0.75000;-0.75000;-0.75000;,
 1.00000;-0.75000;-0.75000;,
 1.00000;0.75000;-0.75000;,
 -1.00000;-0.75000;-0.75000;,
 -0.75000;-0.75000;-0.75000;,
 -1.00000;0.75000;-0.75000;,
 -0.75000;-1.00000;-0.75000;,
 0.75000;-1.00000;-0.75000;,
 -0.75000;0.75000;0.75000;,
 -0.75000;0.75000;-0.75000;,
 -0.75000;1.00000;-0.75000;,
 -0.75000;1.00000;0.75000;,
 -0.75000;-0.75000;-0.75000;,
 -0.75000;-0.75000;-1.00000;,
 -0.75000;0.75000;-1.00000;,
 -0.75000;-0.75000;1.00000;,
 -0.75000;-0.75000;0.75000;,
 -0.75000;0.75000;1.00000;,
 -0.75000;-1.00000;0.75000;,
 -0.75000;-1.00000;-0.75000;,
 0.75000;0.75000;0.75000;,
 -0.75000;0.75000;0.75000;,
 -0.75000;1.00000;0.75000;,
 0.75000;1.00000;0.75000;,
 -0.75000;-0.75000;0.75000;,
 -1.00000;-0.75000;0.75000;,
 -1.00000;0.75000;0.75000;,
 1.00000;-0.75000;0.75000;,
 0.75000;-0.75000;0.75000;,
 1.00000;0.75000;0.75000;,
 0.75000;-1.00000;0.75000;,
 -0.75000;-1.00000;0.75000;,
 0.75000;0.75000;-0.75000;,
 -0.75000;0.75000;-0.75000;,
 -0.75000;0.75000;-1.00000;,
 0.75000;0.75000;-1.00000;,
 -0.75000;0.75000;0.75000;,
 -1.00000;0.75000;0.75000;,
 -1.00000;0.75000;-0.75000;,
 1.00000;0.75000;0.75000;,
 0.75000;0.75000;0.75000;,
 1.00000;0.75000;-0.75000;,
 0.75000;0.75000;1.00000;,
 -0.75000;0.75000;1.00000;,
 0.75000;-0.75000;-1.00000;,
 -0.75000;-0.75000;-1.00000;,
 -0.75000;-0.75000;-0.75000;,
 0.75000;-0.75000;-0.75000;,
 -1.00000;-0.75000;-0.75000;,
 -1.00000;-0.75000;0.75000;,
 -0.75000;-0.75000;0.75000;,
 1.00000;-0.75000;-0.75000;,
 0.75000;-0.75000;0.75000;,
 1.00000;-0.75000;0.75000;,
 -0.75000;-0.75000;1.00000;,
 0.75000;-0.75000;1.00000;,
 -0.93750;-0.75000;-0.47917;,
 -0.93750;-0.75000;-0.35417;,
 -0.93750;0.75000;-0.35417;,
 -0.93750;0.75000;-0.47917;,
 -0.81250;-0.75000;-0.47917;,
 -0.93750;-0.75000;-0.47917;,
 -0.93750;0.75000;-0.47917;,
 -0.81250;0.75000;-0.47917;,
 -0.81250;-0.75000;-0.47917;,
 -0.81250;0.75000;-0.47917;,
 -0.81250;0.75000;-0.35417;,
 -0.81250;-0.75000;-0.35417;,
 -0.81250;-0.75000;-0.35417;,
 -0.81250;0.75000;-0.35417;,
 -0.93750;0.75000;-0.35417;,
 -0.93750;-0.75000;-0.35417;,
 -0.93750;-0.75000;0.35417;,
 -0.93750;-0.75000;0.47917;,
 -0.93750;0.75000;0.47917;,
 -0.93750;0.75000;0.35417;,
 -0.81250;-0.75000;0.35417;,
 -0.93750;-0.75000;0.35417;,
 -0.93750;0.75000;0.35417;,
 -0.81250;0.75000;0.35417;,
 -0.81250;-0.75000;0.35417;,
 -0.81250;0.75000;0.35417;,
 -0.81250;0.75000;0.47917;,
 -0.81250;-0.75000;0.47917;,
 -0.81250;-0.75000;0.47917;,
 -0.81250;0.75000;0.47917;,
 -0.93750;0.75000;0.47917;,
 -0.93750;-0.75000;0.47917;,
 -0.93750;-0.75000;-0.06250;,
 -0.93750;-0.75000;0.06250;,
 -0.93750;0.75000;0.06250;,
 -0.93750;0.75000;-0.06250;,
 -0.81250;-0.75000;-0.06250;,
 -0.93750;-0.75000;-0.06250;,
 -0.93750;0.75000;-0.06250;,
 -0.81250;0.75000;-0.06250;,
 -0.81250;-0.75000;-0.06250;,
 -0.81250;0.75000;-0.06250;,
 -0.81250;0.75000;0.06250;,
 -0.81250;-0.75000;0.06250;,
 -0.81250;-0.75000;0.06250;,
 -0.81250;0.75000;0.06250;,
 -0.93750;0.75000;0.06250;,
 -0.93750;-0.75000;0.06250;,
 0.93750;-0.75000;-0.47917;,
 0.93750;0.75000;-0.47917;,
 0.93750;0.75000;-0.35417;,
 0.93750;-0.75000;-0.35417;,
 0.81250;-0.75000;-0.47917;,
 0.81250;0.75000;-0.47917;,
 0.93750;0.75000;-0.47917;,
 0.93750;-0.75000;-0.47917;,
 0.81250;-0.75000;-0.47917;,
 0.81250;-0.75000;-0.35417;,
 0.81250;0.75000;-0.35417;,
 0.81250;0.75000;-0.47917;,
 0.81250;-0.75000;-0.35417;,
 0.93750;-0.75000;-0.35417;,
 0.93750;0.75000;-0.35417;,
 0.81250;0.75000;-0.35417;,
 0.93750;-0.75000;0.35417;,
 0.93750;0.75000;0.35417;,
 0.93750;0.75000;0.47917;,
 0.93750;-0.75000;0.47917;,
 0.81250;-0.75000;0.35417;,
 0.81250;0.75000;0.35417;,
 0.93750;0.75000;0.35417;,
 0.93750;-0.75000;0.35417;,
 0.81250;-0.75000;0.35417;,
 0.81250;-0.75000;0.47917;,
 0.81250;0.75000;0.47917;,
 0.81250;0.75000;0.35417;,
 0.81250;-0.75000;0.47917;,
 0.93750;-0.75000;0.47917;,
 0.93750;0.75000;0.47917;,
 0.81250;0.75000;0.47917;,
 0.93750;-0.75000;-0.06250;,
 0.93750;0.75000;-0.06250;,
 0.93750;0.75000;0.06250;,
 0.93750;-0.75000;0.06250;,
 0.81250;-0.75000;-0.06250;,
 0.81250;0.75000;-0.06250;,
 0.93750;0.75000;-0.06250;,
 0.93750;-0.75000;-0.06250;,
 0.81250;-0.75000;-0.06250;,
 0.81250;-0.75000;0.06250;,
 0.81250;0.75000;0.06250;,
 0.81250;0.75000;-0.06250;,
 0.81250;-0.75000;0.06250;,
 0.93750;-0.75000;0.06250;,
 0.93750;0.75000;0.06250;,
 0.81250;0.75000;0.06250;,
 -0.75000;-0.75000;-0.75000;,
 -0.75000;-0.75000;0.75000;,
 0.75000;-0.75000;0.75000;,
 0.75000;-0.75000;-0.75000;,
 0.75000;-1.00000;-0.75000;,
 0.75000;-1.00000;0.75000;,
 -0.75000;-1.00000;0.75000;,
 -0.75000;-1.00000;-0.75000;,
 -0.75000;0.75000;-0.75000;,
 0.75000;0.75000;-0.75000;,
 0.75000;0.75000;0.75000;,
 -0.75000;0.75000;0.75000;,
 0.75000;1.00000;-0.75000;,
 -0.75000;1.00000;-0.75000;,
 -0.75000;1.00000;0.75000;,
 0.75000;1.00000;0.75000;,
 -0.34680;-0.48653;0.94904;,
 -0.34680;-0.34680;0.94904;,
 -0.48653;-0.34680;0.94904;,
 -0.48653;-0.48653;0.94904;,
 -0.34680;-0.48653;0.80931;,
 -0.34680;-0.34680;0.80931;,
 -0.34680;-0.34680;0.94904;,
 -0.34680;-0.48653;0.94904;,
 -0.48653;-0.48653;0.80931;,
 -0.48653;-0.34680;0.80931;,
 -0.34680;-0.34680;0.80931;,
 -0.34680;-0.48653;0.80931;,
 -0.48653;-0.48653;0.94904;,
 -0.48653;-0.34680;0.94904;,
 -0.48653;-0.34680;0.80931;,
 -0.48653;-0.48653;0.80931;,
 -0.34680;-0.34680;0.94904;,
 -0.34680;-0.34680;0.80931;,
 -0.48653;-0.34680;0.80931;,
 -0.48653;-0.34680;0.94904;,
 -0.34680;-0.48653;0.80931;,
 -0.34680;-0.48653;0.94904;,
 -0.48653;-0.48653;0.94904;,
 -0.48653;-0.48653;0.80931;,
 -0.34680;0.48653;0.94904;,
 -0.48653;0.48653;0.94904;,
 -0.48653;0.34680;0.94904;,
 -0.34680;0.34680;0.94904;,
 -0.34680;0.48653;0.80931;,
 -0.34680;0.48653;0.94904;,
 -0.34680;0.34680;0.94904;,
 -0.34680;0.34680;0.80931;,
 -0.48653;0.48653;0.80931;,
 -0.34680;0.48653;0.80931;,
 -0.34680;0.34680;0.80931;,
 -0.48653;0.34680;0.80931;,
 -0.48653;0.48653;0.94904;,
 -0.48653;0.48653;0.80931;,
 -0.48653;0.34680;0.80931;,
 -0.48653;0.34680;0.94904;,
 -0.34680;0.34680;0.94904;,
 -0.48653;0.34680;0.94904;,
 -0.48653;0.34680;0.80931;,
 -0.34680;0.34680;0.80931;,
 -0.34680;0.48653;0.80931;,
 -0.48653;0.48653;0.80931;,
 -0.48653;0.48653;0.94904;,
 -0.34680;0.48653;0.94904;,
 0.34680;-0.48653;0.94904;,
 0.48653;-0.48653;0.94904;,
 0.48653;-0.34680;0.94904;,
 0.34680;-0.34680;0.94904;,
 0.34680;-0.48653;0.80931;,
 0.34680;-0.48653;0.94904;,
 0.34680;-0.34680;0.94904;,
 0.34680;-0.34680;0.80931;,
 0.48653;-0.48653;0.80931;,
 0.34680;-0.48653;0.80931;,
 0.34680;-0.34680;0.80931;,
 0.48653;-0.34680;0.80931;,
 0.48653;-0.48653;0.94904;,
 0.48653;-0.48653;0.80931;,
 0.48653;-0.34680;0.80931;,
 0.48653;-0.34680;0.94904;,
 0.34680;-0.34680;0.94904;,
 0.48653;-0.34680;0.94904;,
 0.48653;-0.34680;0.80931;,
 0.34680;-0.34680;0.80931;,
 0.34680;-0.48653;0.80931;,
 0.48653;-0.48653;0.80931;,
 0.48653;-0.48653;0.94904;,
 0.34680;-0.48653;0.94904;,
 0.34680;0.48653;0.94904;,
 0.34680;0.34680;0.94904;,
 0.48653;0.34680;0.94904;,
 0.48653;0.48653;0.94904;,
 0.34680;0.48653;0.80931;,
 0.34680;0.34680;0.80931;,
 0.34680;0.34680;0.94904;,
 0.34680;0.48653;0.94904;,
 0.48653;0.48653;0.80931;,
 0.48653;0.34680;0.80931;,
 0.34680;0.34680;0.80931;,
 0.34680;0.48653;0.80931;,
 0.48653;0.48653;0.94904;,
 0.48653;0.34680;0.94904;,
 0.48653;0.34680;0.80931;,
 0.48653;0.48653;0.80931;,
 0.34680;0.34680;0.94904;,
 0.34680;0.34680;0.80931;,
 0.48653;0.34680;0.80931;,
 0.48653;0.34680;0.94904;,
 0.34680;0.48653;0.80931;,
 0.34680;0.48653;0.94904;,
 0.48653;0.48653;0.94904;,
 0.48653;0.48653;0.80931;,
 -0.34680;-0.48653;-0.94904;,
 -0.48653;-0.48653;-0.94904;,
 -0.48653;-0.34680;-0.94904;,
 -0.34680;-0.34680;-0.94904;,
 -0.34680;-0.48653;-0.80931;,
 -0.34680;-0.48653;-0.94904;,
 -0.34680;-0.34680;-0.94904;,
 -0.34680;-0.34680;-0.80931;,
 -0.48653;-0.48653;-0.80931;,
 -0.34680;-0.48653;-0.80931;,
 -0.34680;-0.34680;-0.80931;,
 -0.48653;-0.34680;-0.80931;,
 -0.48653;-0.48653;-0.94904;,
 -0.48653;-0.48653;-0.80931;,
 -0.48653;-0.34680;-0.80931;,
 -0.48653;-0.34680;-0.94904;,
 -0.34680;-0.34680;-0.94904;,
 -0.48653;-0.34680;-0.94904;,
 -0.48653;-0.34680;-0.80931;,
 -0.34680;-0.34680;-0.80931;,
 -0.34680;-0.48653;-0.80931;,
 -0.48653;-0.48653;-0.80931;,
 -0.48653;-0.48653;-0.94904;,
 -0.34680;-0.48653;-0.94904;,
 -0.34680;0.48653;-0.94904;,
 -0.34680;0.34680;-0.94904;,
 -0.48653;0.34680;-0.94904;,
 -0.48653;0.48653;-0.94904;,
 -0.34680;0.48653;-0.80931;,
 -0.34680;0.34680;-0.80931;,
 -0.34680;0.34680;-0.94904;,
 -0.34680;0.48653;-0.94904;,
 -0.48653;0.48653;-0.80931;,
 -0.48653;0.34680;-0.80931;,
 -0.34680;0.34680;-0.80931;,
 -0.34680;0.48653;-0.80931;,
 -0.48653;0.48653;-0.94904;,
 -0.48653;0.34680;-0.94904;,
 -0.48653;0.34680;-0.80931;,
 -0.48653;0.48653;-0.80931;,
 -0.34680;0.34680;-0.94904;,
 -0.34680;0.34680;-0.80931;,
 -0.48653;0.34680;-0.80931;,
 -0.48653;0.34680;-0.94904;,
 -0.34680;0.48653;-0.80931;,
 -0.34680;0.48653;-0.94904;,
 -0.48653;0.48653;-0.94904;,
 -0.48653;0.48653;-0.80931;,
 0.34680;-0.48653;-0.94904;,
 0.34680;-0.34680;-0.94904;,
 0.48653;-0.34680;-0.94904;,
 0.48653;-0.48653;-0.94904;,
 0.34680;-0.48653;-0.80931;,
 0.34680;-0.34680;-0.80931;,
 0.34680;-0.34680;-0.94904;,
 0.34680;-0.48653;-0.94904;,
 0.48653;-0.48653;-0.80931;,
 0.48653;-0.34680;-0.80931;,
 0.34680;-0.34680;-0.80931;,
 0.34680;-0.48653;-0.80931;,
 0.48653;-0.48653;-0.94904;,
 0.48653;-0.34680;-0.94904;,
 0.48653;-0.34680;-0.80931;,
 0.48653;-0.48653;-0.80931;,
 0.34680;-0.34680;-0.94904;,
 0.34680;-0.34680;-0.80931;,
 0.48653;-0.34680;-0.80931;,
 0.48653;-0.34680;-0.94904;,
 0.34680;-0.48653;-0.80931;,
 0.34680;-0.48653;-0.94904;,
 0.48653;-0.48653;-0.94904;,
 0.48653;-0.48653;-0.80931;,
 0.34680;0.48653;-0.94904;,
 0.48653;0.48653;-0.94904;,
 0.48653;0.34680;-0.94904;,
 0.34680;0.34680;-0.94904;,
 0.34680;0.48653;-0.80931;,
 0.34680;0.48653;-0.94904;,
 0.34680;0.34680;-0.94904;,
 0.34680;0.34680;-0.80931;,
 0.48653;0.48653;-0.80931;,
 0.34680;0.48653;-0.80931;,
 0.34680;0.34680;-0.80931;,
 0.48653;0.34680;-0.80931;,
 0.48653;0.48653;-0.94904;,
 0.48653;0.48653;-0.80931;,
 0.48653;0.34680;-0.80931;,
 0.48653;0.34680;-0.94904;,
 0.34680;0.34680;-0.94904;,
 0.48653;0.34680;-0.94904;,
 0.48653;0.34680;-0.80931;,
 0.34680;0.34680;-0.80931;,
 0.34680;0.48653;-0.80931;,
 0.48653;0.48653;-0.80931;,
 0.48653;0.48653;-0.94904;,
 0.34680;0.48653;-0.94904;;
 
 304;
 3;0,1,2;,
 3;3,4,5;,
 3;6,7,8;,
 3;9,10,11;,
 3;12,13,14;,
 3;15,16,17;,
 3;18,19,20;,
 3;21,22,23;,
 3;24,25,26;,
 3;27,24,26;,
 3;28,29,25;,
 3;24,28,25;,
 3;30,31,29;,
 3;28,30,29;,
 3;32,24,27;,
 3;33,32,27;,
 3;34,30,28;,
 3;35,34,28;,
 3;36,32,33;,
 3;37,36,33;,
 3;38,35,32;,
 3;36,38,32;,
 3;39,34,35;,
 3;38,39,35;,
 3;40,41,42;,
 3;43,40,42;,
 3;44,45,41;,
 3;40,44,41;,
 3;46,47,45;,
 3;44,46,45;,
 3;48,40,43;,
 3;49,48,43;,
 3;50,46,44;,
 3;51,50,44;,
 3;52,48,49;,
 3;53,52,49;,
 3;54,51,48;,
 3;52,54,48;,
 3;55,50,51;,
 3;54,55,51;,
 3;56,57,58;,
 3;59,56,58;,
 3;60,61,57;,
 3;56,60,57;,
 3;62,63,61;,
 3;60,62,61;,
 3;64,56,59;,
 3;65,64,59;,
 3;66,62,60;,
 3;67,66,60;,
 3;68,64,65;,
 3;69,68,65;,
 3;70,67,64;,
 3;68,70,64;,
 3;71,66,67;,
 3;70,71,67;,
 3;72,73,74;,
 3;75,72,74;,
 3;76,77,73;,
 3;72,76,73;,
 3;78,79,77;,
 3;76,78,77;,
 3;80,72,75;,
 3;81,80,75;,
 3;82,78,76;,
 3;83,82,76;,
 3;84,80,81;,
 3;85,84,81;,
 3;86,83,80;,
 3;84,86,80;,
 3;87,82,83;,
 3;86,87,83;,
 3;88,89,90;,
 3;91,88,90;,
 3;92,93,89;,
 3;88,92,89;,
 3;94,95,93;,
 3;92,94,93;,
 3;96,88,91;,
 3;97,96,91;,
 3;98,94,92;,
 3;99,98,92;,
 3;100,96,97;,
 3;101,100,97;,
 3;102,99,96;,
 3;100,102,96;,
 3;103,98,99;,
 3;102,103,99;,
 3;104,105,106;,
 3;107,104,106;,
 3;108,109,105;,
 3;104,108,105;,
 3;110,111,109;,
 3;108,110,109;,
 3;105,112,113;,
 3;106,105,113;,
 3;111,114,115;,
 3;109,111,115;,
 3;112,116,117;,
 3;113,112,117;,
 3;115,118,116;,
 3;112,115,116;,
 3;114,119,118;,
 3;115,114,118;,
 3;120,121,122;,
 3;123,120,122;,
 3;124,125,126;,
 3;121,124,126;,
 3;127,128,120;,
 3;129,127,120;,
 3;130,131,124;,
 3;128,130,124;,
 3;132,133,134;,
 3;135,132,134;,
 3;136,137,138;,
 3;133,136,138;,
 3;139,140,132;,
 3;141,139,132;,
 3;142,143,136;,
 3;140,142,136;,
 3;144,145,146;,
 3;147,144,146;,
 3;148,149,150;,
 3;145,148,150;,
 3;151,152,144;,
 3;153,151,144;,
 3;154,155,148;,
 3;152,154,148;,
 3;156,157,158;,
 3;159,156,158;,
 3;160,161,162;,
 3;157,160,162;,
 3;163,164,156;,
 3;165,163,156;,
 3;166,167,160;,
 3;164,166,160;,
 3;168,169,170;,
 3;171,168,170;,
 3;172,173,174;,
 3;169,172,174;,
 3;175,176,168;,
 3;177,175,168;,
 3;178,179,172;,
 3;176,178,172;,
 3;180,181,182;,
 3;183,180,182;,
 3;182,184,185;,
 3;186,182,185;,
 3;187,183,188;,
 3;189,187,188;,
 3;188,186,190;,
 3;191,188,190;,
 3;192,193,194;,
 3;195,192,194;,
 3;196,197,198;,
 3;199,196,198;,
 3;200,201,202;,
 3;203,200,202;,
 3;204,205,206;,
 3;207,204,206;,
 3;208,209,210;,
 3;211,208,210;,
 3;212,213,214;,
 3;215,212,214;,
 3;216,217,218;,
 3;219,216,218;,
 3;220,221,222;,
 3;223,220,222;,
 3;224,225,226;,
 3;227,224,226;,
 3;228,229,230;,
 3;231,228,230;,
 3;232,233,234;,
 3;235,232,234;,
 3;236,237,238;,
 3;239,236,238;,
 3;240,241,242;,
 3;243,240,242;,
 3;244,245,246;,
 3;247,244,246;,
 3;248,249,250;,
 3;251,248,250;,
 3;252,253,254;,
 3;255,252,254;,
 3;256,257,258;,
 3;259,256,258;,
 3;260,261,262;,
 3;263,260,262;,
 3;264,265,266;,
 3;267,264,266;,
 3;268,269,270;,
 3;271,268,270;,
 3;272,273,274;,
 3;275,272,274;,
 3;276,277,278;,
 3;279,276,278;,
 3;280,281,282;,
 3;283,280,282;,
 3;284,285,286;,
 3;287,284,286;,
 3;288,289,290;,
 3;291,288,290;,
 3;292,293,294;,
 3;295,292,294;,
 3;296,297,298;,
 3;299,296,298;,
 3;300,301,302;,
 3;303,300,302;,
 3;304,305,306;,
 3;307,304,306;,
 3;308,309,310;,
 3;311,308,310;,
 3;312,313,314;,
 3;315,312,314;,
 3;316,317,318;,
 3;319,316,318;,
 3;320,321,322;,
 3;323,320,322;,
 3;324,325,326;,
 3;327,324,326;,
 3;328,329,330;,
 3;331,328,330;,
 3;332,333,334;,
 3;335,332,334;,
 3;336,337,338;,
 3;339,336,338;,
 3;340,341,342;,
 3;343,340,342;,
 3;344,345,346;,
 3;347,344,346;,
 3;348,349,350;,
 3;351,348,350;,
 3;352,353,354;,
 3;355,352,354;,
 3;356,357,358;,
 3;359,356,358;,
 3;360,361,362;,
 3;363,360,362;,
 3;364,365,366;,
 3;367,364,366;,
 3;368,369,370;,
 3;371,368,370;,
 3;372,373,374;,
 3;375,372,374;,
 3;376,377,378;,
 3;379,376,378;,
 3;380,381,382;,
 3;383,380,382;,
 3;384,385,386;,
 3;387,384,386;,
 3;388,389,390;,
 3;391,388,390;,
 3;392,393,394;,
 3;395,392,394;,
 3;396,397,398;,
 3;399,396,398;,
 3;400,401,402;,
 3;403,400,402;,
 3;404,405,406;,
 3;407,404,406;,
 3;408,409,410;,
 3;411,408,410;,
 3;412,413,414;,
 3;415,412,414;,
 3;416,417,418;,
 3;419,416,418;,
 3;420,421,422;,
 3;423,420,422;,
 3;424,425,426;,
 3;427,424,426;,
 3;428,429,430;,
 3;431,428,430;,
 3;432,433,434;,
 3;435,432,434;,
 3;436,437,438;,
 3;439,436,438;,
 3;440,441,442;,
 3;443,440,442;,
 3;444,445,446;,
 3;447,444,446;,
 3;448,449,450;,
 3;451,448,450;,
 3;452,453,454;,
 3;455,452,454;,
 3;456,457,458;,
 3;459,456,458;,
 3;460,461,462;,
 3;463,460,462;,
 3;464,465,466;,
 3;467,464,466;,
 3;468,469,470;,
 3;471,468,470;,
 3;472,473,474;,
 3;475,472,474;,
 3;476,477,478;,
 3;479,476,478;,
 3;480,481,482;,
 3;483,480,482;,
 3;484,485,486;,
 3;487,484,486;,
 3;488,489,490;,
 3;491,488,490;,
 3;492,493,494;,
 3;495,492,494;;
 
 MeshMaterialList {
  7;
  304;
  0,
  0,
  1,
  1,
  2,
  2,
  3,
  3,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6;;
  Material {
   0.133020;0.640000;0.190745;0.996078;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.150582;0.724497;0.215928;;
  }
  Material {
   0.268549;0.155608;0.640000;0.996078;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.331708;0.192205;0.790519;;
  }
  Material {
   0.640000;0.170666;0.032627;0.996078;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.687174;0.183246;0.035032;;
  }
  Material {
   0.640000;0.567216;0.133020;0.996078;;
   5.000000;
   0.000000;0.000000;0.000000;;
   1.170357;1.037258;0.243251;;
  }
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.000000;0.640000;0.640000;0.530000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.800000;0.800000;;
  }
  Material {
   0.640000;0.000000;0.602353;0.500000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.800000;0.000000;0.752941;;
  }
 }
 MeshNormals {
  18;
  -0.565687;0.565687;-0.599998;,
  0.565687;-0.565687;0.599998;,
  -0.565687;-0.565687;-0.599998;,
  0.565687;0.565687;0.599998;,
  0.565687;0.565687;-0.599998;,
  -0.565687;-0.565687;0.599998;,
  0.565687;-0.565687;-0.599998;,
  -0.565687;0.565687;0.599998;,
  0.000000;0.000000;1.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;-0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;;
  304;
  3;0,0,0;,
  3;1,1,1;,
  3;2,2,2;,
  3;3,3,3;,
  3;4,4,4;,
  3;5,5,5;,
  3;6,6,6;,
  3;7,7,7;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;11,11,11;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;9,9,9;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;10,10,10;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;12,12,12;,
  3;11,14,11;,
  3;14,11,11;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,16,9;,
  3;16,9,9;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,14,11;,
  3;14,11,11;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,16,9;,
  3;16,9,9;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,11,11;,
  3;11,11,11;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,9,9;,
  3;9,9,9;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,16,9;,
  3;16,9,9;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,14,11;,
  3;14,11,11;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,16,9;,
  3;16,9,9;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,14,11;,
  3;14,11,11;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;8,17,8;,
  3;17,8,8;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,11,11;,
  3;11,11,11;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,9,9;,
  3;9,9,9;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,11,11;,
  3;11,11,11;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,9,9;,
  3;9,9,9;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,9,9;,
  3;9,9,9;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,11,11;,
  3;11,11,11;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;10,15,10;,
  3;15,10,10;,
  3;9,9,9;,
  3;9,9,9;,
  3;8,17,8;,
  3;17,8,8;,
  3;11,11,11;,
  3;11,11,11;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;12,12,12;,
  3;12,12,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;10,15,10;,
  3;15,10,10;,
  3;11,11,11;,
  3;11,11,11;,
  3;8,17,8;,
  3;17,8,8;,
  3;9,9,9;,
  3;9,9,9;,
  3;13,13,13;,
  3;13,13,13;,
  3;12,12,12;,
  3;12,12,12;;
 }
 MeshTextureCoords {
  496;
  0.500000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.000000;,
  1.000000;0.500000;,
  0.500000;0.500000;,
  0.750000;1.000000;,
  0.500000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.000000;,
  1.000000;0.500000;,
  0.500000;0.500000;,
  0.750000;1.000000;,
  0.500000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.000000;,
  1.000000;0.500000;,
  0.500000;0.500000;,
  0.750000;1.000000;,
  0.500000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.000000;,
  1.000000;0.500000;,
  0.500000;0.500000;,
  0.750000;1.000000;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.000000;,
  1.000000;0.333333;,
  1.000000;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.333333;1.000000;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  1.000000;1.000000;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.000000;,
  1.000000;0.333333;,
  1.000000;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.333333;1.000000;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  1.000000;1.000000;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.000000;,
  1.000000;0.333333;,
  1.000000;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.333333;1.000000;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  1.000000;1.000000;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.000000;,
  1.000000;0.333333;,
  1.000000;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.333333;1.000000;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  1.000000;1.000000;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.000000;,
  1.000000;0.333333;,
  1.000000;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.333333;1.000000;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  1.000000;1.000000;,
  0.333333;1.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;1.000000;,
  0.666667;1.000000;,
  0.666667;0.666667;,
  1.000000;1.000000;,
  1.000000;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.333333;0.000000;,
  0.000000;0.000000;,
  0.666667;0.000000;,
  1.000000;0.000000;,
  0.666667;0.333333;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  1.000000;0.333333;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.666667;0.333333;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  1.000000;0.333333;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.666667;0.333333;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  1.000000;0.333333;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.666667;0.333333;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  1.000000;0.333333;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.666667;0.333333;,
  0.333333;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  1.000000;0.333333;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.666667;1.000000;,
  0.333333;1.000000;,
  0.333333;0.666667;,
  0.666667;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.000000;,
  0.666667;0.000000;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  1.000000;0.666667;,
  1.000000;0.333333;,
  0.666667;0.333333;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.000000;0.333333;,
  0.000000;0.666667;,
  1.000000;0.666667;,
  0.666667;0.666667;,
  0.666667;0.333333;,
  1.000000;0.333333;,
  0.333333;0.666667;,
  0.000000;0.666667;,
  0.000000;0.333333;,
  0.333333;0.333333;,
  0.333333;0.666667;,
  0.333333;0.666667;,
  0.666667;0.666667;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.333333;0.333333;,
  0.333333;0.666667;,
  0.333333;0.666667;,
  0.666667;0.666667;,
  0.666667;0.666667;,
  0.333333;0.666667;,
  0.333333;0.666667;,
  0.333333;0.666667;,
  0.333333;0.333333;,
  0.333333;0.333333;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;;
 }
}
