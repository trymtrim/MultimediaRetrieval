using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using UnityEngine;

// CSV Exporter for shape stats
// To run this, attach it to a game object, change the directoryPath variable from the inspector and run the program
public class ShapeFilter : MonoBehaviour
{
    [SerializeField] private string directoryPath;

    /*[SerializeField]*/ private List<ShapeData> shapeDataList = new List<ShapeData>();

    private void Start()
    {
        shapeDataList = FilterAllShapesFromDatabase(directoryPath);
        WriteShapeDataToCsv(shapeDataList);
        WriteShapeDataDistributionsToCsv(GetShapeDataDistributions(shapeDataList));
    }

    private List<ShapeData> FilterAllShapesFromDatabase(string meshDatabaseDirectoryPath)
    {
        return (from filePath in Directory.GetFiles(meshDatabaseDirectoryPath) where filePath.Contains(".off")
            let shapeBasicData = OffMeshReader.ReadFile(filePath)
            select new ShapeData
            {
                fileName = filePath.Remove(0, filePath.LastIndexOf(Path.DirectorySeparatorChar) + 1),
                vertexCount = shapeBasicData.VertexCount,
                faceCount = shapeBasicData.FaceCount,
                faceType = shapeBasicData.FaceType,
                edgeCount = shapeBasicData.EdgeCount,
                shapeMesh = shapeBasicData.ShapeMesh,
                BoundingBox = shapeBasicData.BoundingBox
            }).ToList();
    }

    private static void WriteShapeDataToCsv(IEnumerable<ShapeData> shapeDataList)
    {
        var csv = new StringBuilder();

        var headerLine = $"File Name,Class,Vertex Count,Face Count,Face Type,Edge Count,Bounding Box Size";
        csv.AppendLine(headerLine);

        foreach (var shapeData in shapeDataList)
        {
            var fileName = shapeData.fileName;
            var className = shapeData.className;
            var vertexCount = shapeData.vertexCount.ToString();
            var faceCount = shapeData.faceCount.ToString();
            var faceType = shapeData.faceType;
            var edgeCount = shapeData.edgeCount.ToString();
            var boundingBox = shapeData.BoundingBox.size.ToString();

            var newLine = $"{fileName},{className},{vertexCount},{faceCount},{faceType},{edgeCount},\"{boundingBox}\"";
            csv.AppendLine(newLine);
        }

        const string filePath = "shape-data.csv";

        File.WriteAllText(filePath, csv.ToString());
    }

    private static ShapeDataDistributions GetShapeDataDistributions(List<ShapeData> shapeDataList)
    {
        var averageVertexCount = 0;
        var averageFaceCount = 0;
        var faceTypesPresent = string.Empty;
        var averageEdgeCount = 0;
        var averageBoundingBoxSize = Vector3.zero;

        int[] vertexCountRange = {int.MaxValue, 0};
        int[] faceCountRange = {int.MaxValue, 0};
        int[] edgeCountRange = {int.MaxValue, 0};
        Vector3[] boundingBoxSizeRange =
            {new Vector3(float.MaxValue, float.MaxValue, float.MaxValue), new Vector3(0.0f, 0.0f, 0.0f)};

        foreach (var shapeData in shapeDataList)
        {
            averageVertexCount += shapeData.vertexCount;
            averageFaceCount += shapeData.faceCount;

            if (!faceTypesPresent.Contains(shapeData.faceType))
                faceTypesPresent += $"{shapeData.faceType}";

            averageEdgeCount += shapeData.edgeCount;
            averageBoundingBoxSize += shapeData.BoundingBox.size;

            if (shapeData.vertexCount < vertexCountRange[0])
                vertexCountRange[0] = shapeData.vertexCount;
            if (shapeData.vertexCount > vertexCountRange[1])
                vertexCountRange[1] = shapeData.vertexCount;

            if (shapeData.faceCount < faceCountRange[0])
                faceCountRange[0] = shapeData.faceCount;
            if (shapeData.vertexCount > faceCountRange[1])
                faceCountRange[1] = shapeData.faceCount;

            if (shapeData.edgeCount < edgeCountRange[0])
                edgeCountRange[0] = shapeData.edgeCount;
            if (shapeData.edgeCount > edgeCountRange[1])
                edgeCountRange[1] = shapeData.edgeCount;

            if (shapeData.BoundingBox.size.magnitude < boundingBoxSizeRange[0].magnitude)
                boundingBoxSizeRange[0] = shapeData.BoundingBox.size;
            if (shapeData.BoundingBox.size.magnitude > boundingBoxSizeRange[1].magnitude)
                boundingBoxSizeRange[1] = shapeData.BoundingBox.size;
        }

        averageVertexCount /= shapeDataList.Count;
        averageFaceCount /= shapeDataList.Count;
        averageEdgeCount /= shapeDataList.Count;
        averageBoundingBoxSize /= shapeDataList.Count;

        return new ShapeDataDistributions
        {
            AverageVertexCount = averageVertexCount,
            AverageFaceCount = averageFaceCount,
            FaceTypesPresent = faceTypesPresent,
            AverageEdgeCount = averageEdgeCount,
            AverageBoundingBoxSize = averageBoundingBoxSize,

            VertexCountRange = $"{vertexCountRange[0]} - {vertexCountRange[1]}",
            FaceCountRange = $"{faceCountRange[0]} - {faceCountRange[1]}",
            EdgeCountRange = $"{edgeCountRange[0]} - {edgeCountRange[1]}",
            BoundingBoxSizeRange = $"{boundingBoxSizeRange[0]} - {boundingBoxSizeRange[1]}"
        };
    }

    static void WriteShapeDataDistributionsToCsv(ShapeDataDistributions shapeDataDistributions)
    {
        var csv = new StringBuilder();

        var headerLine =
            $"Average Vertex Count,Average Face Count,Face Types Present,Average Edge Count,Average Bounding Box Size,Vertex Count Range,Face Count Range,Edge Count Range,Bounding Box Size Range";
        csv.AppendLine(headerLine);

        var newLine =
            $"{shapeDataDistributions.AverageVertexCount},{shapeDataDistributions.AverageFaceCount},{shapeDataDistributions.FaceTypesPresent}," +
            $"{shapeDataDistributions.AverageEdgeCount},\"{shapeDataDistributions.AverageBoundingBoxSize}\",\"{shapeDataDistributions.VertexCountRange}\"," +
            $"\"{shapeDataDistributions.FaceCountRange}\",\"{shapeDataDistributions.EdgeCountRange}\",\"{shapeDataDistributions.BoundingBoxSizeRange}\"";
        csv.AppendLine(newLine);

        const string filePath = "shape-data-distributions.csv";

        File.WriteAllText(filePath, csv.ToString());
    }
}

[Serializable]
public struct ShapeData
{
    public string fileName;
    public string className;
    public int vertexCount;
    public int faceCount;
    public string faceType;
    public int edgeCount;

    public Mesh shapeMesh;
    public Bounds BoundingBox;
}

public struct ShapeDataDistributions
{
    public int AverageVertexCount;
    public int AverageFaceCount;
    public string FaceTypesPresent;
    public int AverageEdgeCount;
    public Vector3 AverageBoundingBoxSize;

    public string VertexCountRange;
    public string FaceCountRange;
    public string EdgeCountRange;
    public string BoundingBoxSizeRange;
}