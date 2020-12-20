using System.Collections.Generic;
using UnityEngine;

public class ShapeViewer : MonoBehaviour
{
    [Header("Prefabs")] [SerializeField] private GameObject shapeRenderImagePrefab;
    [Header("Scene")] [SerializeField] private RectTransform viewportContent;
    [SerializeField] private Transform shapeCameraContainer;
    [SerializeField] private CameraRenderTexture queryShapeRenderImage;
    [SerializeField] private GameObject shapeViewerUI;
    [SerializeField] private OffMeshReader inspectedShape;
    [SerializeField] private CameraMovement cameraMovement;
    [SerializeField] private GameObject inspectionUI;
    [SerializeField] private GameObject globalDirectionalLight;
    [SerializeField] private GameObject cameraDirectionalLight;

    private Vector2 _viewportContentOriginalSize;

    private void Start()
    {
        _viewportContentOriginalSize = viewportContent.sizeDelta;
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
            StopInspectingShape();

        if (!inspectionUI.activeInHierarchy) return;

        if (Input.GetKeyDown(KeyCode.Space) &&
            inspectedShape.gameObject.GetComponent<StaticWireframeRenderer>() == null)
            inspectedShape.gameObject.AddComponent<StaticWireframeRenderer>();
        else if (Input.GetKeyDown(KeyCode.Space))
            Destroy(inspectedShape.gameObject.GetComponent<StaticWireframeRenderer>());
    }

    private void ResetViewer()
    {
        foreach (Transform child in viewportContent)
            Destroy(child.gameObject);

        foreach (Transform child in shapeCameraContainer)
            Destroy(child.gameObject);

        viewportContent.sizeDelta = _viewportContentOriginalSize;
    }

    private void ShowShapes(IReadOnlyList<string> shapeFilePaths, string queryShapeFilePath)
    {
        ResetViewer();

        queryShapeRenderImage.Initialize(-1, queryShapeFilePath, this);

        const float rowHeight = 250.0f;
        const int shapesPerRow = 5;
        const int rowsInView = 3;

        var counter = 0;
        var windowHeight = rowHeight;

        for (var i = 0; i < shapeFilePaths.Count; i++)
        {
            var shapeFilePath = shapeFilePaths[i];

            if (string.IsNullOrEmpty(shapeFilePath))
                continue;

            // Cheap fix
            if (shapeFilePath.Contains("./"))
                shapeFilePath = shapeFilePath.Replace("./", "Backend/");

            var shapeRenderImage = Instantiate(shapeRenderImagePrefab, viewportContent);
            shapeRenderImage.GetComponent<CameraRenderTexture>().Initialize(i, shapeFilePath, this);

            counter++;

            if (counter != shapesPerRow) continue;

            counter = 0;
            windowHeight += rowHeight;
        }

        if (shapeFilePaths.Count > shapesPerRow * rowsInView)
            viewportContent.sizeDelta = new Vector2(viewportContent.sizeDelta.x, windowHeight);

        viewportContent.localPosition = new Vector2(0.0f, 0.0f);
    }

    public void InspectShape(string shapeFilePath)
    {
        shapeViewerUI.SetActive(false);
        inspectionUI.SetActive(true);
        cameraMovement.enabled = true;
        inspectedShape.gameObject.SetActive(true);
        cameraDirectionalLight.SetActive(true);
        globalDirectionalLight.SetActive(false);

        cameraMovement.ResetCamera();

        inspectedShape.RenderMesh(OffMeshReader.ReadFile(shapeFilePath).ShapeMesh);
    }

    private void StopInspectingShape()
    {
        if (shapeViewerUI.activeInHierarchy)
            return;

        shapeViewerUI.SetActive(true);
        inspectionUI.SetActive(false);
        cameraMovement.enabled = false;
        inspectedShape.gameObject.SetActive(false);
        cameraDirectionalLight.SetActive(false);
        globalDirectionalLight.SetActive(true);

        if (inspectedShape.gameObject.GetComponent<StaticWireframeRenderer>() != null)
            Destroy(inspectedShape.gameObject.GetComponent<StaticWireframeRenderer>());
    }

    public void SelectShape()
    {
        SimpleFileBrowser.FileBrowser.ShowLoadDialog(SelectShapeFromPath, null);
    }

    public void AddShapeToDatabase()
    {
        SimpleFileBrowser.FileBrowser.ShowLoadDialog(AddShapeToDatabaseFromPath, null);
    }

    private void SelectShapeFromPath(string[] paths)
    {
        var shapeFilePath = paths[0];
        var result = Backend.Query(shapeFilePath);

        ShowShapes(result.OutputData, shapeFilePath);
    }

    private void AddShapeToDatabaseFromPath(string[] paths)
    {
        print("Adding shape to database:\n" + paths[0]);

        var shapeFilePath = paths[0];
        Backend.Store(shapeFilePath, Util.GetDatabasePath());

        print("Finished adding shape to database:\n" + paths[0]);
    }
}