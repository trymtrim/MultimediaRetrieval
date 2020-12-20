using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using UnityEngine;

//[ExecuteInEditMode]
[RequireComponent(typeof(MeshRenderer))]
[RequireComponent(typeof(MeshFilter))]
public class OffMeshReader : MonoBehaviour
{
    [SerializeField] private Material material;

    public static ShapeBasicData ReadFile(string filePath)
    {
        var shapeBasicData = new ShapeBasicData();
        var mesh = new Mesh();

        try
        {
            var reader = new StreamReader(filePath, Encoding.UTF8);

            var vertices = new List<Vector3>();
            var triangles = new List<int>();

            var line = "";
            int i;

            // Reading the number of data
            for (i = 0; i < 2; ++i)
            {
                line = reader.ReadLine();
            }
            
            System.Diagnostics.Debug.Assert(line != null, nameof(line) + " != null");
            var numbers = line.Split(' ');

            var numVertex = int.Parse(numbers[0], CultureInfo.InvariantCulture.NumberFormat);
            var numFaces = int.Parse(numbers[1], CultureInfo.InvariantCulture.NumberFormat);
            var numEdges = int.Parse(numbers[2], CultureInfo.InvariantCulture.NumberFormat);

            shapeBasicData.VertexCount = numVertex;
            shapeBasicData.FaceCount = numFaces;
            shapeBasicData.EdgeCount = numEdges;

            //Vertex
            for (i = 0; i < numVertex; ++i)
            {
                line = reader.ReadLine();
                System.Diagnostics.Debug.Assert(line != null, nameof(line) + " != null");
                var vertex = line.Split(' ');

                var x = float.Parse(vertex[0], CultureInfo.InvariantCulture.NumberFormat);
                var y = float.Parse(vertex[1], CultureInfo.InvariantCulture.NumberFormat);
                var z = float.Parse(vertex[2], CultureInfo.InvariantCulture.NumberFormat);

                vertices.Add(new Vector3(x, y, z));
            }

            var hasTris = false;
            var hasQuads = false;

            //Triangle
            for (i = 0; i < numFaces; ++i)
            {
                line = reader.ReadLine();
                System.Diagnostics.Debug.Assert(line != null, nameof(line) + " != null");
                var triangle = line.Split(' ');

                triangles.Add(int.Parse(triangle[1], CultureInfo.InvariantCulture.NumberFormat));
                triangles.Add(int.Parse(triangle[2], CultureInfo.InvariantCulture.NumberFormat));
                triangles.Add(int.Parse(triangle[3], CultureInfo.InvariantCulture.NumberFormat));

                switch (int.Parse(triangle[0], CultureInfo.InvariantCulture.NumberFormat))
                {
                    case 3:
                        hasTris = true;
                        break;
                    case 4:
                        hasQuads = true;
                        break;
                }
            }

            if (hasTris && hasQuads)
                shapeBasicData.FaceType = "Triangles/Quads";
            else if (hasTris)
                shapeBasicData.FaceType = "Triangles";
            else if (hasQuads)
                shapeBasicData.FaceType = "Quads";

            reader.Close();

            //Mesh setting
            mesh.vertices = vertices.ToArray();
            mesh.triangles = triangles.ToArray();

            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
        }
        catch (System.Exception e)
        {
            Debug.Log(e.Message);
        }

        shapeBasicData.ShapeMesh = mesh;
        shapeBasicData.BoundingBox = mesh.bounds;

        return shapeBasicData;
    }

    public void RenderMesh(Mesh mesh)
    {
        GetComponent<MeshFilter>().sharedMesh = mesh;
        GetComponent<MeshRenderer>().material = material;
    }
}

public struct ShapeBasicData
{
    public int VertexCount;
    public int FaceCount;
    public string FaceType;
    public int EdgeCount;
    public Mesh ShapeMesh;
    public Bounds BoundingBox;
}