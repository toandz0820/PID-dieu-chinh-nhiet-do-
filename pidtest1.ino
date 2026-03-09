// --- KHAI BÁO BIẾN ---
double Setpoint = 50.0;  // Mục tiêu vẫn là 50 độ
double Input = 25.0;     // Nhiệt độ hiện tại của vật
double Output = 0.0;     // Công suất lò sưởi

// --- BIẾN MÔI TRƯỜNG ---
double Nhiet_do_moi_truong = 25.0; // Mặc định ban đầu là 25 độ

// Hệ số PID (Giữ nguyên hoặc chỉnh tùy ý)
double Kp = 8.0; 
double Ki = 0.5;
double Kd = 5.0;

double last_error = 0;
double integral = 0;

void setup() {
  Serial.begin(9600);
  // Quan trọng: Đặt thời gian chờ nhập liệu cực ngắn
  // để đồ thị không bị khựng lại khi mày gõ phím
  Serial.setTimeout(10); 
}

void loop() {
  // --- 1. NHẬN LỆNH TỪ BÀN PHÍM (Real-time) ---
  // Kiểm tra xem mày có gửi số nào xuống không
  if (Serial.available() > 0) {
    // Đọc số thực (float) mày gửi
    float gia_tri_nhap = Serial.parseFloat();
    
    // Nếu gõ số hợp lệ (khác 0) thì cập nhật môi trường
    // (Tao chặn số 0 để tránh lỗi trôi dòng khi Enter nhầm)
    if (gia_tri_nhap != 0) {
      Nhiet_do_moi_truong = gia_tri_nhap;
    }
    
    // Xóa bộ nhớ đệm để tránh đọc lại rác
    while(Serial.available() > 0) { char t = Serial.read(); }
  }

  // --- 2. TÍNH TOÁN PID (Như cũ) ---
  double error = Setpoint - Input;
  
  integral += error;
  if(integral > 255) integral = 255; 
  if(integral < -255) integral = -255;
  
  double P = Kp * error;
  double I = Ki * integral;
  double D = Kd * (error - last_error);
  
  Output = P + I + D;
  
  if (Output > 255) Output = 255;
  if (Output < 0) Output = 0;
  
  last_error = error;

  // --- 3. GIẢ LẬP VẬT LÝ ---
  // Môi trường càng lạnh, nhiệt mất càng nhanh
  double mat_nhiet = (Input - Nhiet_do_moi_truong) * 0.05;
  Input = Input + (Output * 0.1) - mat_nhiet;

  // --- 4. VẼ ĐỒ THỊ ---
  Serial.print(Setpoint);             // Xanh dương (Mục tiêu)
  Serial.print(" ");      
  Serial.print(Nhiet_do_moi_truong);  // Cam (Môi trường - Mày điều khiển cái này)
  Serial.print(" ");
  Serial.println(Input);              // Đỏ/Xanh lá (Nhiệt độ thật)

  delay(50);
}