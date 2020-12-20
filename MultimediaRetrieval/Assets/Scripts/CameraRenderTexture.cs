using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(Image))]
public class CameraRenderTexture : MonoBehaviour
{
    [SerializeField] private GameObject shapeCameraPrefab;

    public void Initialize(int gridIndex, string shapeFilePath, ShapeViewer shapeViewer)
    {
        var rt = new RenderTexture(256, 256, 16, RenderTextureFormat.ARGB32) {name = "ShapeRenderTexture" + gridIndex};
        rt.Create();

        var material = new Material(Shader.Find("Sprites/Diffuse")) {mainTexture = rt};

        GetComponent<Image>().material = material;

        var shapeCamera = Instantiate(shapeCameraPrefab, new Vector3(gridIndex * 250, 1000, 0), Quaternion.identity,
            GameObject.Find("ShapeCameraContainer").transform);

        shapeCamera.GetComponent<Camera>().targetTexture = rt;
        var mesh = OffMeshReader.ReadFile(shapeFilePath).ShapeMesh;
        shapeCamera.GetComponentInChildren<OffMeshReader>().RenderMesh(mesh);

        GetComponent<Button>().onClick.AddListener(delegate { shapeViewer.InspectShape(shapeFilePath); });
    }
}