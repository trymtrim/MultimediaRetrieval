using UnityEngine;

public class CameraMovement : MonoBehaviour
{
    [SerializeField] private Camera _camera;
    [SerializeField] private Transform targetObject;

    private float _currentX, _currentY;

    private const bool InvertPanDirection = false;
    private const float Speed = 2;
    private const float TranslationSpeed = 0.1f;
    private const float ZoomSpeed = 6;

    private void Update()
    {
        if (Input.GetMouseButton(0))
        {
            _currentX -= Input.GetAxis("Mouse Y") * Speed;
            _currentX = Mathf.Clamp(_currentX, 0.0f, 90.0f);

            _currentY += Input.GetAxis("Mouse X") * Speed;
        }

        if (Input.GetKeyDown(KeyCode.R))
            ResetCamera();

        var cameraPosition = _camera.transform.position;
        var transformPosition = transform.position;

        var zoomingIn = Input.GetAxis("Mouse ScrollWheel") < 0.0f &&
                        Vector3.Distance(transformPosition, cameraPosition) < 20.0f;

        var zoomingOut = Input.GetAxis("Mouse ScrollWheel") > 0.0f &&
                         Vector3.Distance(transformPosition, cameraPosition) > 1.5f;

        if (zoomingIn || zoomingOut)
        {
            var zoomDelta = Vector3.MoveTowards(cameraPosition,
                                new Vector3(transformPosition.x, transformPosition.y - 2, transformPosition.z), 1.0f) *
                            (Time.deltaTime * ZoomSpeed);

            if (zoomingIn)
                _camera.transform.position += zoomDelta;
            else
                _camera.transform.position -= zoomDelta;
        }

        var targetObjectPosition = targetObject.position;

        transform.rotation = Quaternion.Euler(new Vector3(_currentX, _currentY, transform.rotation.z));
        targetObject.rotation = Quaternion.Euler(new Vector3(targetObjectPosition.x, 0, targetObjectPosition.z));

        var cameraRotation = _camera.transform.rotation;

        var panDirection = 1;

        // ReSharper disable once ConditionIsAlwaysTrueOrFalse
        if (InvertPanDirection)
            // ReSharper disable once HeuristicUnreachableCode
#pragma warning disable 162
            panDirection *= -1;
#pragma warning restore 162

        targetObject.Translate(cameraRotation *
                               new Vector3(Input.GetAxis("Horizontal") * TranslationSpeed * panDirection, 0, 0));
        targetObject.Translate(cameraRotation *
                               new Vector3(0, Input.GetAxis("Vertical") * TranslationSpeed * panDirection, 0));
    }

    public void ResetCamera()
    {
        _camera.transform.localPosition = new Vector3(0.0f, 0.0f, -1.5f);
        _currentX = 0.0f;
        _currentY = 0.0f;
        targetObject.position = new Vector3(0.0f, 0.0f, 0.0f);
    }
}