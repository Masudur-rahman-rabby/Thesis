import cv2
import mediapipe as mp
import numpy as np

# Initialize Mediapipe Face Mesh
mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp_face_mesh.FaceMesh(refine_landmarks=True)

# Initialize Video Capture
cap = cv2.VideoCapture(0)

# Variables
blink_count = 0  # Blink counter for toggling movement
movement = "Stop"  # Initially STOP
blink_detected = False  # Prevent multiple counts for one blink

# Reference points for head and eye tracking
reference_head_x = None  # Head center position reference
reference_eye_x = None  # Eye center position reference
current_head_x = 0
current_eye_x = 0

# EAR Threshold for Blink Detection
EAR_THRESHOLD = 0.18

def calculate_ear(eye_landmarks, img_h):
    """Calculate Eye Aspect Ratio (EAR) for blink detection."""
    top = abs(eye_landmarks[1].y - eye_landmarks[5].y) * img_h
    bottom = abs(eye_landmarks[2].y - eye_landmarks[4].y) * img_h
    width = abs(eye_landmarks[0].x - eye_landmarks[3].x) * img_h
    return (top + bottom) / (2.0 * width)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)  # Mirror effect
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_mesh.process(rgb_frame)

    if results.multi_face_landmarks:
        for face_landmarks in results.multi_face_landmarks:
            img_h, img_w, _ = frame.shape

            # Extract eye landmarks for EAR calculation
            right_eye_pts = [face_landmarks.landmark[i] for i in [33, 160, 158, 133, 153, 144]]
            left_eye_pts = [face_landmarks.landmark[i] for i in [362, 385, 387, 263, 373, 380]]

            # Calculate EAR for blink detection
            left_ear = calculate_ear(left_eye_pts, img_h)
            right_ear = calculate_ear(right_eye_pts, img_h)

            # Blink detection
            is_blinking = (left_ear + right_ear) / 2.0 < EAR_THRESHOLD

            if is_blinking and not blink_detected:
                blink_count += 1
                blink_detected = True  # Prevent multiple counts
                movement = "Forward" if blink_count % 2 == 1 else "Stop"

            elif not is_blinking:
                blink_detected = False  # Reset flag when eyes open

            # Head tracking for movement
            right_cheek_x = int(face_landmarks.landmark[234].x * img_w)
            left_cheek_x = int(face_landmarks.landmark[454].x * img_w)
            current_head_x = (right_cheek_x + left_cheek_x) // 2

            # Eye tracking for movement
            right_eye_x = int(face_landmarks.landmark[33].x * img_w)
            left_eye_x = int(face_landmarks.landmark[362].x * img_w)
            current_eye_x = (right_eye_x + left_eye_x) // 2

            # Set reference points initially
            if reference_head_x is None:
                reference_head_x = current_head_x

            if reference_eye_x is None:
                reference_eye_x = current_eye_x

            # Compute displacement from reference positions
            head_displacement = current_head_x - reference_head_x
            eye_displacement = current_eye_x - reference_eye_x

            head_threshold = abs(left_cheek_x - right_cheek_x) * 0.15  # 15% of face width
            eye_threshold = abs(left_eye_x - right_eye_x) * 0.10  # 10% of eye width

            # Movement logic based on head position
            if movement != "Stop":  # Only track movement when not in STOP mode
                if head_displacement > head_threshold:
                    movement = "Head Right"
                elif head_displacement < -head_threshold:
                    movement = "Head Left"
                else:
                    movement = "Forward"

                # Eye movement logic (only if head is relatively stable)
                if abs(head_displacement) < head_threshold / 2:  # Ensure head is not moving much
                    if eye_displacement > eye_threshold:
                        movement = "Eye Right"
                    elif eye_displacement < -eye_threshold:
                        movement = "Eye Left"

    # Display information
    cv2.putText(frame, f"Movement: {movement}", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    cv2.putText(frame, f"Blink Count: {blink_count}", (50, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

    # Ensure visualization only when face is detected
    if results.multi_face_landmarks and reference_head_x is not None and reference_eye_x is not None:
        cv2.circle(frame, (current_head_x, int(img_h / 2)), 5, (255, 0, 0), -1)  # Head tracking point
        cv2.circle(frame, (current_eye_x, int(img_h / 2)), 5, (0, 255, 0), -1)  # Eye tracking point

    cv2.imshow("Eye & Head Tracking", frame)

    key = cv2.waitKey(1)
    if key == 27:  # ESC key
        break

cap.release()
cv2.destroyAllWindows()
