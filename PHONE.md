# 📱 Dùng SmartGPS trên điện thoại

Ứng dụng chạy trên máy tính và **mọi điện thoại trong cùng mạng Wi-Fi** đều dùng được (không cần cài app store).

---

## 1. Bật server

Tại thư mục dự án:

```powershell
docker compose -f docker/docker-compose.yml up -d --build
```

Kiểm tra: mở `http://localhost:8080` trên máy tính thấy trang đăng nhập là OK.

---

## 2. Lấy địa chỉ cho điện thoại

Chạy:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\phone-url.ps1
```

Script sẽ:
- in ra **URL dạng `http://192.168.x.x:8080`**
- mở cổng **TCP 8080** trên Windows Firewall (sẽ hiện hộp thoại xin quyền Admin — bấm **Yes**)

> Nếu IP máy tính thay đổi (Wi-Fi khác, DHCP cấp lại), chạy lại script. Thêm `-Update` để cập nhật luôn link hiển thị trong app:
> ```powershell
> powershell -ExecutionPolicy Bypass -File scripts\phone-url.ps1 -Update
> ```

Cách thủ công: mở `cmd` → `ipconfig` → tìm **IPv4 Address** của card Wi-Fi (ví dụ `192.168.1.13`) → trên điện thoại mở `http://192.168.1.13:8080`.

---

## 3. Mở trên điện thoại

1. Điện thoại và máy tính **cùng một Wi-Fi** (không dùng 4G, không bật "AP isolation" trên router).
2. Mở Chrome (Android) hoặc Safari (iPhone), nhập URL ở bước 2.
3. Đăng nhập:
   - Sinh viên: `B22DCCN001` / `password123`
   - Giảng viên: `GV101` / `password123`
   - Admin: `ADM001` / `admin123`

### Cài như app (khuyến nghị)

| Android (Chrome) | iPhone (Safari) |
|---|---|
| Menu ⋮ → **Add to Home screen** | Nút Chia sẻ → **Add to Home Screen** |
| Mở từ icon, chạy toàn màn hình | Mở từ icon, chạy toàn màn hình |

App có sẵn `manifest.webmanifest` + icon + service worker nên cài được như ứng dụng thật.

---

## 4. Lưu ý về GPS trên điện thoại

Trình duyệt **chỉ cho phép GPS trên HTTPS** (hoặc `localhost`). Khi mở bằng `http://192.168.x.x:8080`:

- Chrome/Safari sẽ **không** trả về vị trí thật.
- App tự chuyển sang **toạ độ lớp học (chế độ demo)** để bạn vẫn demo được luồng điểm danh, và hiện banner vàng giải thích.

Muốn GPS thật trên điện thoại cần HTTPS. Có 2 cách:

1. **Dùng tunnel có HTTPS** (nhanh nhất, cần internet):
   ```powershell
   # ví dụ với cloudflared
   cloudflared tunnel --url http://localhost:8080
   ```
   Mở link `https://...trycloudflare.com` trên điện thoại → GPS hoạt động.

2. **Cài chứng chỉ tự ký** (offline): thêm nginx/caddy làm TLS termination và tin cậy chứng chỉ trên điện thoại.

Với mục đích demo trên lớp, chế độ toạ độ lớp học là đủ: điểm danh vẫn ghi nhận đúng, radar và thống kê vẫn cập nhật thật.

---

## 5. Quy tắc điểm danh (chỉ được dùng cùng Wi-Fi với máy chủ)

Để chống gian lận, hệ thống **chỉ chấp nhận điểm danh khi điện thoại đang kết nối cùng mạng Wi-Fi** với máy tính chạy server:

| Tình huống | Mở trang web | Điểm danh |
|---|---|---|
| Cùng Wi-Fi với máy chủ | ✅ Mở được | ✅ Thành công |
| 4G / Wi-Fi khác / ngoài lớp | ✅ Mở được (nếu có internet) | ❌ Bị chặn, hiện thông báo |
| HTTPS tunnel (cloudflared/ngrok) cùng lớp | ✅ Mở được | ✅ Thành công (app tự học IP lớp học lần đầu) |

**Lý do:** server kiểm tra địa chỉ IP của thiết bị gọi API. Nếu IP không nằm trong dải mạng lớp học (`CLASSROOM_CIDR`, mặc định `192.168.1.0/24`, `172.16.0.0/12`, `127.0.0.0/8`), yêu cầu điểm danh sẽ trả về **HTTP 403** và không ghi nhận điểm.

Nếu bạn dùng tunnel HTTPS (cloudflared) để mở từ ngoài, hãy **lần đầu mở trang web trên một thiết bị đang ở trong lớp cùng Wi-Fi** — app sẽ ghi lại IP công khai của lớp, sau đó các thiết bị cùng lớp qua tunnel vẫn điểm danh được.

---

## 5.1. Ghi nhớ thiết bị & cảnh cáo điểm danh hộ

Lần đầu bấm **Điểm danh**, app hiện hộp thoại xin xác nhận: **"Cho phép ghi nhớ thiết bị phần cứng"**.

- Bấm **Đồng ý & điểm danh**: trình duyệt tạo và lưu một mã thiết bị ẩn danh (ví dụ `DEV-EE27834E-4504`) → điểm danh được gửi lên server.
- Bấm **Từ chối**: điểm danh bị hủy, không gửi dữ liệu lên server.

Mỗi tài khoản chỉ được gắn với 1 mã thiết bị. Nếu dùng **cùng một thiết bị cho tài khoản sinh viên thứ hai** để điểm danh hộ, hệ thống sẽ:

1. Chặn yêu cầu điểm danh (không tính điểm).
2. Hiện banner **⚠️ CẢNH CÁO: Phát hiện điểm danh hộ!** kèm mã thiết bị.
3. Ghi sự việc vào `data/audit.txt` với loại `PROXY_ATTEMPT` (severity `CRITICAL`).

Admin màn hình 8 mục **2. Device Binding Management** giờ đọc thiết bị thật từ `data/students.txt` (mã thiết bị + trạng thái khóa + nút Reset Lock) thay vì dữ liệu mẫu. Sinh viên ở màn hình GPS có nút **"Yêu cầu reset thiết bị"** gửi đơn kèm ảnh chứng minh (JPG/PNG, ≤5 MB, giống mẫu xin nghỉ); Admin duyệt ở mục **6. Yêu cầu mở khóa thiết bị** → hệ thống tự gỡ liên kết thiết bị cũ và ghi `DEVICE_RESET` vào audit. Dữ liệu đơn được lưu tại `data/device_reset_requests.txt`.

---

## 6. Xử lý sự cố

| Hiện tượng | Cách xử lý |
|---|---|
| Điện thoại không mở được | Kiểm tra cùng Wi-Fi; chạy lại `scripts\phone-url.ps1` (firewall + IP) |
| Mở được nhưng trang trắng/không style | Điện thoại cần internet để tải Tailwind/FontAwesome từ CDN |
| `docker ps` chỉ thấy `8080/tcp` | `docker compose -f docker/docker-compose.yml down` rồi `up -d` |
| Đổi Wi-Fi, link cũ chết | Chạy `scripts\phone-url.ps1 -Update` |
| GPS không chạy | Bình thường với HTTP — xem mục 4 |
| Bấm điểm danh bị chặn (403) | Đảm bảo điện thoại cùng Wi-Fi với máy chủ; xem mục 5 |
