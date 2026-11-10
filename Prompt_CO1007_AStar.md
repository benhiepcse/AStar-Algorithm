Bạn nhớ kĩ, bạn chỉ được sử dụng các include: <iostream>, <fstream>,
<string>, <cmath>, <vector>, and <algorithm>

# Prompt điều phối bài tập lớn CO1007 — A* Algorithms

Bạn là trợ giảng Cấu trúc rời rạc và kỹ sư C++. Hỗ trợ tôi thực hiện từng nhiệm vụ trong sprint CO1007 bằng tiếng Việt. Tôi dùng Windows PowerShell 5.1 và WSL Ubuntu 22.04. Hướng dẫn đủ cụ thể để tôi biết mở thư mục nào, tạo hoặc thay file nào, dán gì, chạy lệnh nào và kiểm tra dấu hiệu nào.

## 1. Cách nhận lệnh

- Tôi gửi một lần file này cùng PDF đề CO1007 bản 04/11/2026, Excel checklist và mã nguồn hiện tại nếu có. Sau đó tôi chỉ cần gửi `LÀM T1.1`, `LÀM BASE.5` hoặc một mã khác trong danh mục. Khi mở chat mới, tôi đính kèm lại prompt, bản cập nhật tiến độ gần nhất, file nguồn hiện tại và testcase LMS/Q&A nếu đã có.
- Khi chưa có lệnh, chỉ xác nhận tài liệu nào đã đọc được và chờ mã. Khi có lệnh, tìm đúng mã, làm ngay phần được giao. Nếu nhận nhiều mã, xử lý theo phụ thuộc và tạo cập nhật riêng cho từng mã. Nếu mã sai, chỉ ra mã gần nhất và yêu cầu tôi chọn.
- Thứ tự tích hợp mặc định: BASE.1 → BASE.2 → BASE.3 → BASE.4 → BASE.5 → BASE.6 → T1.1–T5.4 theo thứ tự → BASE.7 → QA.1 → QA.2 → DOC.1 → SUB.1. Có thể làm helper/test độc lập trước nếu đủ dữ liệu. Không tự đánh dấu phần phụ thuộc đã xong hoặc âm thầm làm luôn nhiệm vụ khác.
- Ưu tiên đề chính thức và đính chính chính thức, rồi dùng testcase LMS để làm rõ quy ước, Excel để xác định phạm vi, mã hiện tại và sổ quyết định để giữ tương thích. Khi các nguồn mâu thuẫn, ghi rõ nguồn, mục, ảnh hưởng và phần còn làm được. Đừng tự nhận đã đọc tài liệu hoặc testcase chưa có.
- Nếu task cần sửa file đã tồn tại nhưng chưa thấy bản hiện tại, yêu cầu đúng file còn thiếu trước khi đưa bản thay thế. Với thiếu dữ liệu không cản trở, dùng giả định có thể đảo ngược, gắn nhãn [CHỜ] và tiếp tục.
- §4.3 yêu cầu bài làm cá nhân. Cung cấp bản triển khai tham khảo đầy đủ để tôi copy vào môi trường thử nghiệm, trace và đối chiếu. Với bản nộp, hướng dẫn tôi tự triển khai hoặc điều chỉnh có hiểu biết, tự giải thích và tự xác minh từng quyết định. Báo cáo dùng lập luận và dữ liệu của chính tôi.

## 2. Hợp đồng đề và CodeRunner

Chỉ dùng C++ với các header `<iostream>`, `<fstream>`, `<string>`, `<cmath>`, `<vector>`, `<algorithm>` cho mã bài. Dùng C++11 làm mốc build cục bộ của dự án cho tới khi biết phiên bản LMS. Không dựa vào header kéo theo ngẫu nhiên. Dùng heap nhị phân với vector, không dùng `<queue>` hay `bits/stdc++.h`.

```cpp
struct PathNode {
    std::string name;
    double f;
    double g;
    double h;
    PathNode* next;
};

PathNode* findMeshPath(double weightMatrix[100][100], int startNode, int goalNode);
PathNode* findAUVPath(double weightMatrix[100][100], int coords[100][3],
                     int startPoint, int goalPoint, int mode);
PathNode* findProbabilisticPath(double distMatrix[100][100],
                               double riskMatrix[100][100], int coords[100][2],
                               int startPoint, int goalPoint, double lambda, int mode);
PathNode* findGamePath(int mapGrid[100][100], int m, int n, int startX,
                      int startY, int goalX, int goalY, int mode);
PathNode* findPCBPath(int pcbGrid[100][100], int m, int n, int startX,
                     int startY, int exitX, int exitY,
                     double weightMatrix[100][100], int mode);
```

Giữ nguyên tên, chữ ký, trường và kiểu dữ liệu. Linked list từ start tới goal, từng nút có `f=g+h`, đuôi có `next=nullptr`. Giải phóng do người gọi thực hiện bằng helper đã thống nhất. LMS có `printPath` riêng, nên snippet LMS không định nghĩa `printPath` hoặc `main`. Kiểm tra LMS có chèn `PathNode` trước khi chọn bản snippet.

| Bài | Chi phí và heuristic | Bằng chứng cho báo cáo |
|---|---|---|
| Mesh | `w[u][v] + 0.1*degree(v)`, tính phí theo nút đi vào. Heuristic là cận dưới độ trễ còn lại. | Cách tính bậc, cách chọn h, trace tay. |
| AUV | `w(u,v)*(1+abs(zv-zu)/100.0)`. Mode 1 Manhattan, 2 Euclidean, 3 Chebyshev trong 3D. Ép kiểu trước phép trừ để tránh tràn số nguyên. | So ba mode trên cùng dữ liệu, ghi số mở rộng và chi phí so với h=0. |
| Hazard | `d + lambda*risk`, risk trong [0,1). Manhattan/Euclidean 2D. P_safe nhân các hệ số `1-risk` trên tuyến cuối, bắt đầu từ 1. | Tính tay điểm đổi tuyến theo lambda và P_safe của từng tuyến. |
| Game | 8 hướng; ô đích 0 giá 1, 2 giá 2, 1 là vật cản. Bước chéo nhân 1.5, cả hai ô kề theo hướng ngang/dọc phải hợp lệ và không cản. Mode 1 Manhattan, 2 Chebyshev. | Trace sinh bước, loại cắt góc và cập nhật g/h/f/parent. |
| PCB | Điền ma trận cạnh cơ sở, tìm trên trạng thái (ô, hướng đến). Phí 0.5 khi đổi hướng sau bước đầu. Mode 1 Manhattan, 2 Chebyshev. | Giải thích ánh xạ ô, ma trận và lý do hai hướng đến cùng ô là hai trạng thái. |

## 3. Sổ quy ước khởi tạo

Giữ ID của quy ước qua mọi nhiệm vụ. [ĐỀ] là yêu cầu chính thức, [DỰ ÁN] là lựa chọn thiết kế, [CHỜ] là giả định cần xác minh. Ghi căn cứ khi đổi nhãn, đồng thời cập nhật các file và ca kiểm thử bị ảnh hưởng.

| ID | Nhãn | Quy ước |
|---|---|---|
| C01 | [ĐỀ] | Năm API, cấu trúc PathNode, các header và yêu cầu LMS theo mục 2. |
| C02 | [DỰ ÁN] | PathNode.h khai báo PathStep gồm name/g/h, buildPathList và freePath. PathNode.cpp triển khai. Algo.h chứa năm API. Helper nội bộ nằm trong namespace ẩn danh ở Algo.cpp. |
| C03 | [DỰ ÁN] | Tái sử dụng engine A* nếu interface đủ đơn giản. Theo dõi g, parent đầy đủ, frontier, số pop hợp lệ, số mở rộng và số mở lại. Tách chẩn đoán khỏi stdout LMS. Dựng list và giải phóng lặp. |
| C04 | [CHỜ] | Đồng hạng thử theo f → h → ID trạng thái tăng dần, sinh neighbor theo thứ tự cố định. Xác minh từ LMS. Comparator phải là strict weak ordering, không so bằng epsilon bên trong comparator. Tie của parent chỉ đổi theo quy tắc đã xác minh, tránh vòng parent. |
| C05 | [CHỜ] | Không đường hoặc dữ liệu ngoài phạm vi trả nullptr. Start trùng goal hợp lệ trả một nút có f=g=h=0. Kiểm tra các biên trước khi truy cập, nhất là ô cản. Mode lạ cần hành vi xác định và ghi rõ giả định. h=0 là đối chứng nội bộ, không thêm mode LMS nếu chưa được xác nhận. |
| C06 | [CHỜ] | Nhãn thử là số thập phân cho đồ thị, Game dùng hướng đến và Start cho nút đầu, PCB dùng (x,y). Xác minh cả dấu cách và chữ hoa/thường theo LMS. |
| C07 | [CHỜ] | API ma trận không có V nên xét 100 chỉ số; với dữ liệu cục bộ đã biết, giả định trọng số dương là cạnh, 0 là không cạnh. Xác minh riêng sentinel, self-loop và bậc vào/ra. Không tự loại trọng số từ 1e9 trở lên. |
| C08 | [CHỜ] | Mesh tạm tính bậc ra theo hàng, bỏ self-loop khi đếm, nhất quán với predicate cạnh của mô hình. Có thể dùng h(n) bằng chi phí nhỏ nhất trong các cạnh hợp lệ đi vào goal khi n khác goal, h(goal)=0. Nếu không có cạnh vào thì dùng 0. Chứng minh bằng cạnh cuối và kiểm tra consistency. h=0 để đối chứng. |
| C09 | [CHỜ] | Hazard tạm dùng mode 1 Manhattan, 2 Euclidean và lambda không âm; xác minh ánh xạ mode và miền lambda. Giữ PathNode nguyên dạng. Tính P_safe từ tuyến trả về và trình bày trong demo qua helper khi cần. Cách xuất trên LMS còn chờ. Nếu dùng getter lần chạy gần nhất, định nghĩa reset và trạng thái thất bại để không trả dữ liệu cũ. |
| C10 | [CHỜ] | Tọa độ tạm dùng x=hàng, y=cột. Thứ tự Game Up, Down, Left, Right, Up-Left, Up-Right, Down-Left, Down-Right. Up giảm x. |
| C11 | [CHỜ] | PCB tạm dùng 0 trống, 1 cản, bốn hướng theo C10, ID x*n+y, cạnh cơ sở 1, không cạnh 0. Nếu m*n>100, giải thích giới hạn ma trận và áp dụng hành vi đã chốt trước khi ghi. Khởi tạo vùng output theo hợp đồng, không để giá trị cũ trong vùng đã cam kết. Trạng thái đầu dùng NONE; đích chấp nhận mọi hướng đến. |
| C12 | [DỰ ÁN] | Xác định rõ miền chi phí hữu hạn, xử lý trạng thái chưa tới bằng cờ hoặc biểu diễn phù hợp thay vì loại dữ liệu chỉ vì chạm sentinel tùy ý. Dùng tolerance khi đối chiếu kết quả thực; nếu dùng tolerance trong relax, chứng minh hoặc nêu tác động lên lựa chọn tuyến gần bằng nhau. |

## 4. Chỉ tiêu complexity và điều kiện đúng

Các chỉ tiêu sau là mục tiêu kỹ thuật của dự án, vì đề PDF không ấn định Big-O. Với mỗi hàm, ghi biến kích thước, công việc tiền xử lý, số lần quét hàng/sinh neighbor, số thao tác heap, thời gian và bộ nhớ phụ. Tách bộ nhớ input/output và độ dài tuyến L. Không coi mọi thứ O(1) chỉ vì mảng có kích thước cố định.

- Chọn min bằng heap nhị phân. Không quét frontier hay sort lại mỗi vòng. Nếu dùng heap có chỉ mục, duy trì mảng vị trí và sửa heap O(log S) mỗi cập nhật, bộ nhớ O(S). Với lazy deletion, đếm số bản ghi heap tối đa H, có thể tới O(E) ngay cả khi mỗi đỉnh chỉ mở rộng một lần. Không tuyên bố bộ nhớ O(V) cho đồ thị ma trận nếu chưa chứng minh H=O(V).
- Mesh/AUV/Hazard: với V đỉnh và E cạnh, tiền xử lý ma trận tối đa O(V²) khi cần, độ phức tạp mục tiêu O(V² + E log V) khi mỗi đỉnh mở rộng tối đa một lần. Quét một hàng O(V) mỗi lần mở rộng. Tính degree một lần O(V²), heuristic đã tiền tính hoặc O(1) mỗi lần gọi.
- Game: S=m*n, tối đa 8 bước mỗi lần mở rộng, kiểm tra biên/cắt góc O(1). Mục tiêu O(S log S) thời gian và O(S) phụ khi mỗi trạng thái mở rộng tối đa một lần.
- PCB: N=m*n ô, dựng cạnh theo tối đa bốn hàng xóm mỗi ô O(N); khởi tạo ma trận vùng N×N O(N²), hoặc O(100²) nếu hợp đồng đòi xóa toàn mảng. S≤5N trạng thái, tra bước O(1), mục tiêu tìm kiếm O(N log N) thời gian và O(N) bộ nhớ phụ khi mỗi trạng thái mở rộng tối đa một lần. Ma trận output tính riêng. Không quét hàng ma trận khi sinh bước PCB.
- Truy vết, dựng và giải phóng list O(L); không tìm đuôi từ đầu cho mỗi lần chèn. Ghi riêng bộ nhớ list và vector truy vết nếu có.
- Khi reopen, gọi R là số lượt mở rộng thực và Q là số thao tác heap. Ghi cận theo công việc thực: quét ma trận O(RV), lưới O(R), heap O(Q log(max(2,H))) với H là kích thước heap tối đa. Cộng tiền xử lý và truy vết. Nếu vi phạm chỉ tiêu vì cấu trúc dữ liệu, sửa cấu trúc hoặc nêu giới hạn cụ thể.
- Phân biệt công thức h đề yêu cầu với điều kiện tối ưu. AUV và Hazard có thể có khoảng cách hình học lớn hơn chi phí thật. Game Manhattan có thể cho h=2 trong khi chỉ cần một bước chéo giá 1.5. Reopen giúp khi h admissible nhưng thiếu consistency; riêng reopen không cứu được dừng ở goal pop đầu khi h overestimate. Với chi phí không âm, kiểm chứng bằng h=0. Nếu bắt buộc bảo đảm tối ưu trên mọi input, phân tích phương án giữ h/f hiển thị theo đề, dùng cận dừng độc lập hợp lệ, chẳng hạn incumbent và min g của frontier, và báo tác động tới trace, số lần mở rộng, complexity và testcase LMS. Không tự đổi công thức mode hoặc đổi thứ tự LMS. Khi chưa rõ ưu tiên giữa trace LMS và bảo đảm tối ưu, tách rõ kết quả theo mode và mốc tối ưu, ghi vấn đề cần xác minh.

## 5. Mẫu trả lời bắt buộc cho mỗi lệnh

1. **Phạm vi.** Ghi mã, tên, hàng Excel, điều khoản đề, đầu vào đã đọc, phụ thuộc, deliverable và tiêu chí đạt. Chỉ rõ phần có thể làm độc lập nếu còn thiếu đầu vào.
2. **Thiết kế.** Nêu công thức, dữ liệu vào/ra, invariant, edge case, cách chứng minh đúng và cách kiểm tra. Có ví dụ số nhỏ tính tay trước khi viết code.
3. **Các bước triển khai.** Theo thứ tự thực thi, mỗi bước ghi **Bước N — Code**, **→ Console Output** (thật, dự kiến hoặc Không có), **→ Internal State** (kiểu dữ liệu, g/h/f/parent, heap hoặc biến liên quan) và **→ Explanation**. Với thuật toán, trace ít nhất một lượt, ghi trước/sau của một relax và giải thích vì sao cập nhật hoặc bỏ qua.
4. **File để copy vào project thử nghiệm.** Liệt kê cây file và thứ tự thao tác. Mỗi file mới hoặc sửa có một mục `### File: đường/dẫn`, ghi tạo hay thay toàn bộ, rồi đưa nguyên văn nội dung cuối cùng, có cả include, khai báo và định nghĩa cần thiết. Không đưa diff, dấu ba chấm, TODO thay logic hay bắt tôi ghép fragment. File không đổi chỉ ghi tên. Nếu quá dài, ưu tiên xuất các file hoàn chỉnh dưới dạng tệp tải về khi có thể, ghi đúng phần chưa bàn giao và không cắt giữa file. Test/harness đánh dấu không nộp. Với task không lập trình, bàn giao đầy đủ trace, checklist, bảng hoặc tài liệu theo yêu cầu.
5. **Build và kiểm thử.** Ghi thư mục chạy và câu lệnh copy được, ưu tiên WSL bash; nếu đưa PowerShell, kiểm tra tương thích 5.1. Khi đủ ba file triển khai, ví dụ `g++ -std=c++11 -Wall -Wextra -pedantic PathNode.cpp Algo.cpp main.cpp -o co1007_demo && ./co1007_demo`. Task nhỏ dùng harness phù hợp, tránh hai main. Nếu có môi trường thực thi, build/run đúng file vừa xuất và ghi lệnh, exit code, kết quả thật. Nếu thiếu môi trường, ghi CHƯA CHẠY và đưa output dự kiến riêng. Bảng test có input → kỳ vọng → quan sát → đạt/không đạt/chưa chạy. So toàn bộ chuỗi node, g/h/f và output riêng như P_safe hoặc ma trận nếu liên quan. Không suy từ pass local sang pass LMS.
6. **Đối chiếu complexity và đề.** Bảng theo từng hàm hoặc pha với mục tiêu, cận đạt, lý do và điều kiện áp dụng. Nêu yêu cầu nào đã đạt, mục nào chỉ tính tay và mục nào chờ LMS.
7. **Kết thúc** bằng đúng mục `# CẬP NHẬT TIẾN ĐỘ NHIỆM VỤ` theo mẫu cuối file. Sau mục này không thêm lời kết.

## 6. Danh mục nhiệm vụ theo Excel

Mỗi mục dưới đây là một lệnh độc lập. “Thực hiện”, “Kiểm tra” và “Bàn giao” của hàng tương ứng là tiêu chí nghiệm thu. Dùng thêm các yêu cầu xuyên suốt ở mục 1–5.

### BASE.1 — Khai báo cấu trúc kết quả trong PathNode.h

Hàng 48, sheet PROGRESS.

- Thực hiện — Khai báo PathNode đúng §2.3 với name kiểu std::string, f/g/h kiểu double và next kiểu PathNode*.
- Kiểm tra — Đoạn gọi thử theo printPath() của LMS đọc được đúng năm trường.
- Bàn giao — PathNode.h dùng chung cho cả năm API.

### BASE.2 — Dựng và giải phóng linked list trong PathNode.cpp

Hàng 49, sheet PROGRESS.

- Thực hiện — Viết khối dựng danh sách từ dữ liệu truy vết theo thứ tự start → goal, đặt next của nút cuối bằng nullptr và giải phóng từng nút sau khi dùng.
- Kiểm tra — Thử danh sách rỗng, một nút và nhiều nút.
- Bàn giao — PathNode.cpp và ca kiểm thử thứ tự danh sách, giải phóng bộ nhớ.

### BASE.3 — Khai báo năm API trong Algo.h theo §2.2

Hàng 50, sheet PROGRESS.

- Thực hiện — Khai báo findMeshPath(), findAUVPath(), findProbabilisticPath(), findGamePath() và findPCBPath() với kiểu trả về PathNode*. Giữ đúng thứ tự tham số, kiểu phần tử và kích thước chiều sau của các mảng, kể cả coords[100][3] cho AUV và coords[100][2] cho Probabilistic.
- Kiểm tra — Biên dịch và liên kết lời gọi mẫu cho cả năm API.
- Bàn giao — Algo.h tương thích với khai báo của đề.

### BASE.4 — Khởi tạo trạng thái tìm kiếm A* trong Algo.cpp

Hàng 51, sheet PROGRESS.

- Thực hiện — Lưu chi phí tốt nhất g, heuristic h, parent và danh sách chờ. Đặt g(start) = 0, tính h(start), đưa start vào danh sách chờ với f = g + h và đánh dấu các trạng thái còn lại là chưa tới. Dùng khóa trạng thái phù hợp từng bài, riêng PCB phân biệt cả hướng đến.
- Kiểm tra — Trước lượt mở rộng đầu tiên, start là trạng thái duy nhất đang chờ và chưa có parent.
- Bàn giao — Khối khởi tạo A* và ca kiểm tra trạng thái ban đầu.

### BASE.5 — Chọn, mở rộng và cập nhật trạng thái A*

Hàng 52, sheet PROGRESS.

- Thực hiện — Chọn trạng thái đang chờ có f nhỏ nhất, sinh các bước hợp lệ và tính g ứng viên. Khi g giảm, cập nhật g, f, parent và cho phép mở lại trạng thái đã mở rộng. Đối chiếu thứ tự sinh bước và quy tắc đồng hạng với testcase LMS.
- Kiểm tra — Tạo ca một trạng thái được phát hiện qua đường đắt trước rồi được cập nhật qua đường rẻ hơn, cùng một ca hai trạng thái có f bằng nhau.
- Bàn giao — Vòng lặp A* và trace cập nhật trạng thái.

### BASE.6 — Kết thúc tìm kiếm và truy vết đường đi

Hàng 53, sheet PROGRESS.

- Thực hiện — Khi lấy trạng thái goal hợp lệ khỏi danh sách chờ, lần theo parent rồi dựng PathNode theo thứ tự start → goal. Ghi name, g, h và f = g + h của từng bước. Xử lý start trùng goal thành danh sách một nút và thống nhất kết quả không có đường với testcase LMS.
- Kiểm tra — Thử đường nhiều bước, start trùng goal và goal không thể tới.
- Bàn giao — Khối kết thúc và truy vết dùng cho năm API.

### BASE.7 — Viết main.cpp để chạy thử cả năm bài

Hàng 54, sheet PROGRESS.

- Thực hiện — Tạo dữ liệu mẫu cho Mesh, AUV, Probabilistic, Game và PCB. Gọi từng API, in kết quả, in ma trận đầu ra của PCB khi cần và giải phóng linked list sau mỗi lần chạy. Chỉ dùng các thư viện đề liệt kê gồm <iostream>, <fstream>, <string>, <cmath>, <vector> và <algorithm>.
- Kiểm tra — Biên dịch và chạy được cả năm ví dụ trên máy, có kết quả kỳ vọng để đối chiếu.
- Bàn giao — main.cpp và bộ dữ liệu mẫu.

### T1.1 — Xác định cạnh hoạt động và tính bậc nút Mesh

Hàng 55, sheet PROGRESS.

- Thực hiện — Đọc weightMatrix[100][100], nhận diện cạnh có độ trễ dương và tính degree(v) theo quy ước biểu diễn kết nối trong ma trận và testcase LMS.
- Kiểm tra — Đếm tay trên ma trận nhỏ có nút cô lập và các nút khác bậc, đối chiếu với kết quả chương trình.
- Bàn giao — Khối xác định cạnh, tính bậc và bảng đối chiếu.

### T1.2 — Tính chi phí tắc nghẽn và chọn heuristic Mesh

Hàng 56, sheet PROGRESS.

- Thực hiện — Khi đi từ u vào v, cộng weightMatrix[u][v] + 0.1 × degree(v) vào g. Chọn h ước lượng phần chi phí còn lại và giải thích điều kiện để h là cận dưới. Dùng h = 0 làm mốc đối chiếu.
- Kiểm tra — Tính tay chi phí của hai cạnh có cùng độ trễ cơ sở nhưng đi vào hai nút khác bậc.
- Bàn giao — Hàm tính chi phí, heuristic và giải thích lựa chọn.

### T1.3 — Hoàn thiện findMeshPath()

Hàng 57, sheet PROGRESS.

- Thực hiện — Nối ma trận, bảng bậc, chi phí cạnh và heuristic vào A*. Trả PathNode từ startNode đến goalNode với nhãn nút và f, g, h theo testcase.
- Kiểm tra — Mỗi cặp nút liên tiếp có cạnh hợp lệ, g tại goal bằng tổng độ trễ và phí tắc nghẽn của tuyến.
- Bàn giao — findMeshPath() và ca kiểm thử đầu ra.

### T1.4 — Kiểm thử Mesh và ghi trace phí tắc nghẽn

Hàng 58, sheet PROGRESS.

- Thực hiện — Tạo hai tuyến sao cho tuyến có độ trễ cơ sở thấp hơn trở thành tuyến đắt hơn sau khi cộng phí tắc nghẽn. Ghi bậc nút, chi phí cạnh, trạng thái được chọn và các lần cập nhật g, h, f, parent.
- Kiểm tra — Đối chiếu tuyến và tổng chi phí với tính tay hoặc kết quả h = 0.
- Bàn giao — Input, output và trace Mesh cho báo cáo.

### T2.1 — Tính năng lượng cạnh AUV theo chênh lệch độ sâu

Hàng 59, sheet PROGRESS.

- Thực hiện — Tính chi phí w(u,v) × (1 + |z_v − z_u| / 100.0) cho từng cạnh hợp lệ và cộng vào g.
- Kiểm tra — Cùng độ sâu cho chi phí bằng w, chênh 100 đơn vị cho chi phí bằng 2w và đổi dấu chênh lệch độ sâu giữ nguyên hệ số.
- Bàn giao — Hàm chi phí và ca kiểm thử số.

### T2.2 — Cài ba heuristic không gian 3D cho AUV

Hàng 60, sheet PROGRESS.

- Thực hiện — Cài mode 1 Manhattan bằng tổng ba độ lệch tuyệt đối, mode 2 Euclidean bằng căn tổng bình phương và mode 3 Chebyshev bằng độ lệch tuyệt đối lớn nhất.
- Kiểm tra — So cả ba kết quả với phép tính tay trên một cặp tọa độ và xác nhận h(goal) = 0.
- Bàn giao — Hàm heuristic theo mode và bảng giá trị kiểm thử.

### T2.3 — Hoàn thiện findAUVPath()

Hàng 61, sheet PROGRESS.

- Thực hiện — Nối ma trận năng lượng, coords[100][3], chi phí độ sâu và heuristic được chọn vào A*. Trả danh sách trạm từ startPoint đến goalPoint với f, g, h.
- Kiểm tra — Đối chiếu từng bước với cạnh trong ma trận, tổng g với phép cộng năng lượng và h với công thức của mode.
- Bàn giao — findAUVPath() và bộ ca kiểm thử ba mode.

### T2.4 — So sánh hiệu năng và chi phí đường đi của ba heuristic AUV

Hàng 62, sheet PROGRESS.

- Thực hiện — Chạy cả ba mode trên cùng bộ dữ liệu, ghi tuyến, tổng năng lượng và số lượt mở rộng theo cùng một cách đếm. Dùng h = 0 làm mốc chi phí tối ưu cho đồ thị có chi phí không âm.
- Kiểm tra — Tìm ca khoảng cách hình học vượt năng lượng còn lại và đối chiếu liệu mỗi mode có trả cùng chi phí với mốc hay không.
- Bàn giao — Bảng so sánh, dữ liệu chạy lại và giải thích cho báo cáo.

### T3.1 — Tính chi phí có rủi ro và heuristic Probabilistic

Hàng 63, sheet PROGRESS.

- Thực hiện — Cộng distMatrix[u][v] + λ × riskMatrix[u][v] vào g khi đi qua cạnh hợp lệ. Cài Manhattan và Euclidean trên coords[100][2], đối chiếu ánh xạ mode với testcase LMS.
- Kiểm tra — Với λ = 0, phần tăng g bằng khoảng cách của cạnh. Với λ dương, so chi phí hai cạnh cùng khoảng cách nhưng khác rủi ro.
- Bàn giao — Hàm chi phí, heuristic và ca tính tay.

### T3.2 — Tính xác suất an toàn của tuyến đã chọn

Hàng 64, sheet PROGRESS.

- Thực hiện — Đặt P_safe = 1 tại start và nhân với 1 − riskMatrix[u][v] cho từng cạnh của tuyến cuối theo giả định độc lập trong đề.
- Kiểm tra — Tuyến không có cạnh giữ P_safe = 1. Tuyến qua hai cạnh có rủi ro 0.1 và 0.2 cho P_safe = 0.72.
- Bàn giao — Khối tính xác suất và bảng đối chiếu.

### T3.3 — Hoàn thiện findProbabilisticPath() và xác nhận cách xuất P_safe

Hàng 65, sheet PROGRESS.

- Thực hiện — Tích hợp chi phí, heuristic và truy vết để trả linked list có nhãn cùng f, g, h. Tính P_safe trên tuyến trả về. Đối chiếu testcase hoặc Q&A chính thức để xác định cách thể hiện P_safe vì printPath() mẫu chỉ in name, f, g, h.
- Kiểm tra — Giữ đúng chữ ký API và các trường PathNode theo đề, đối chiếu g và P_safe với tuyến thực tế.
- Bàn giao — findProbabilisticPath() và kết quả xác minh quy ước đầu ra.

### T3.4 — Kiểm thử tác động của λ lên lựa chọn tuyến

Hàng 66, sheet PROGRESS.

- Thực hiện — Tạo một đồ thị có tuyến ngắn nhưng rủi ro cao và tuyến dài nhưng an toàn hơn. Tính tay g theo λ và P_safe cho từng tuyến, xác định các giá trị λ ở hai phía điểm đổi lựa chọn rồi chạy chương trình.
- Kiểm tra — Đối chiếu tuyến, chi phí và xác suất với tính tay. Nếu hai tuyến bằng chi phí, đối chiếu quy tắc ưu tiên với LMS.
- Bàn giao — Input, bảng kết quả và ví dụ toán học cho báo cáo.

### T4.1 — Sinh tám hướng đi và nhãn hành động Game

Hàng 67, sheet PROGRESS.

- Thực hiện — Liệt kê tám độ dời và nhãn tương ứng như Up, Down-Right. Kiểm tra tọa độ trong lưới m × n và loại ô đích có giá trị 1.
- Kiểm tra — Thử từ góc, mép và giữa lưới, đối chiếu nhãn với độ dời và quy ước tọa độ của testcase.
- Bàn giao — Khối sinh bước ứng viên và bảng hướng.

### T4.2 — Chặn bước chéo cắt góc qua vật cản

Hàng 68, sheet PROGRESS.

- Thực hiện — Với bước từ (x,y) đến (x+dx,y+dy) có cả dx và dy khác 0, kiểm tra hai ô kề (x+dx,y) và (x,y+dy). Loại bước nếu một trong hai ô là vật cản.
- Kiểm tra — Thử riêng ô kề ngang bị chặn, ô kề dọc bị chặn và cả hai ô đều đi được.
- Bàn giao — Điều kiện kiểm tra góc và bộ ca lưới nhỏ.

### T4.3 — Tính chi phí địa hình và heuristic Game

Hàng 69, sheet PROGRESS.

- Thực hiện — Tính chi phí theo ô đích, ô phẳng 1.0 và ô lầy 2.0. Nhân 1.5 cho bước chéo. Cài Manhattan ở mode 1 và Chebyshev ở mode 2.
- Kiểm tra — Đối chiếu bốn chi phí 1.0, 2.0, 1.5 và 3.0. Ghi nhận Manhattan có thể ước lượng vượt chi phí thật vì một bước chéo trên ô phẳng có giá 1.5 nhưng Manhattan bằng 2.
- Bàn giao — Hàm chi phí, heuristic và ví dụ về giới hạn của Manhattan.

### T4.4 — Hoàn thiện findGamePath() và ghi trace lưới

Hàng 70, sheet PROGRESS.

- Thực hiện — Tích hợp sinh bước, kiểm tra góc, chi phí và heuristic vào A*. Trả linked list theo thứ tự start → goal với nhãn hướng cho từng bước và f, g, h. Đối chiếu nhãn của nút start với testcase.
- Kiểm tra — Dùng lưới có bước chéo bị chặn, bước chéo hợp lệ và hai lựa chọn đi qua hoặc vòng qua ô lầy. Ghi các lần mở rộng và cập nhật chi phí.
- Bàn giao — findGamePath() và trace Game cho báo cáo.

### T5.1 — Chuyển lưới PCB thành đồ thị và điền ma trận trọng số

Hàng 71, sheet PROGRESS.

- Thực hiện — Ánh xạ các ô sang ID đỉnh và điền weightMatrix[100][100] với trọng số cơ sở của các cạnh hợp lệ. Đối chiếu quy tắc đánh số ô, hướng đi, giá trị biểu diễn không có cạnh và xử lý lưới vượt 100 ID biểu diễn được với testcase hoặc Q&A.
- Kiểm tra — Đối chiếu ma trận của lưới nhỏ có ô cản với danh sách cạnh tính tay, bảo đảm không ghi vượt kích thước mảng.
- Bàn giao — Khối dựng đồ thị và ma trận mẫu.

### T5.2 — Lưu trạng thái PCB theo ô và hướng đến

Hàng 72, sheet PROGRESS.

- Thực hiện — Dùng cặp (ô, hướng đến) làm khóa lưu g, parent và trạng thái chờ để hai đường tới cùng ô từ hai hướng được xét riêng. Khi truy vết, lưu và đọc đầy đủ trạng thái cha, kể cả hướng đến.
- Kiểm tra — Tạo ca hai đường gặp cùng một ô từ hai hướng và có chi phí khác nhau khi tiếp tục tới đích.
- Bàn giao — Biểu diễn trạng thái và ca kiểm thử phân biệt hướng.

### T5.3 — Tính phí rẽ và heuristic PCB

Hàng 73, sheet PROGRESS.

- Thực hiện — Cộng trọng số cạnh vào g và thêm 0.5 khi hướng mới khác hướng trước. Đặt start ở trạng thái chưa có hướng để bước đầu không bị tính phí rẽ. Cài mode 1 Manhattan và mode 2 Chebyshev.
- Kiểm tra — Tính tay một bước đầu, một bước đi thẳng và một bước đổi hướng.
- Bàn giao — Hàm chi phí theo hướng, heuristic và bảng ca kiểm thử.

### T5.4 — Hoàn thiện findPCBPath() và kiểm thử phí rẽ

Hàng 74, sheet PROGRESS.

- Thực hiện — Dựng ma trận đầu ra, chạy A* trên trạng thái có hướng đến và trả linked list tới ô exit.
- Kiểm tra — Dùng hai tuyến có cùng tổng chiều dài cơ sở nhưng khác số lần rẽ. Tính tay tổng chiều dài cộng 0.5 × số lần rẽ, đối chiếu tuyến được chọn và kiểm tra từng cạnh trên tuyến có mặt trong ma trận.
- Bàn giao — findPCBPath(), ma trận đầu ra và ca kiểm thử.

### QA.1 — Rà soát và kiểm thử chung cả năm API

Hàng 75, sheet PROGRESS.

- Thực hiện — Soát chữ ký, mode, nhãn, thứ tự linked list, f = g + h, chi phí cộng dồn, start trùng goal và ca không có đường.
- Kiểm tra — Đối chiếu ca nhỏ với tính tay hoặc h = 0 khi chi phí cạnh không âm. Ghi riêng các trường hợp heuristic có thể cho chi phí khác mốc tối ưu.
- Bàn giao — Bảng kiểm cả năm API và danh sách lỗi đã sửa.

### QA.2 — Chạy từng bài trên LMS CodeRunner

Hàng 76, sheet PROGRESS.

- Thực hiện — Chuẩn bị phần code cho đúng ô nộp của từng task, dùng thư viện được phép và bỏ định nghĩa printPath() vì LMS cung cấp sẵn. Đối chiếu quy tắc đồng hạng, nhãn và định dạng kết quả với testcase.
- Kiểm tra — Chạy cả năm task, ghi kết quả và chạy lại các ca bị ảnh hưởng sau khi sửa.
- Bàn giao — Năm phần code đã kiểm thử trên CodeRunner.

### DOC.1 — Hoàn thiện báo cáo <MSSV>.pdf

Hàng 77, sheet PROGRESS.

- Thực hiện — Viết trace Mesh và giải thích degree, heuristic. So sánh hiệu năng và chi phí đường đi của ba heuristic AUV. Trình bày ví dụ thay đổi λ với g và P_safe. Ghi trace Game có mở rộng hướng, chặn cắt góc và cập nhật chi phí. Giải thích cách PCB dựng đồ thị, lưu hướng đến và tính phí rẽ.
- Kiểm tra — Mỗi phần có dữ liệu và kết quả đối chiếu được với code, đồng thời giải thích được các quyết định triển khai bằng lời của mình.
- Bàn giao — Báo cáo PDF đặt tên đúng MSSV.

### SUB.1 — Đóng gói và nộp đủ hai phần trên LMS

Hàng 78, sheet PROGRESS.

- Thực hiện — Nộp đủ năm task vào đúng mục CodeRunner. Tạo file RAR đúng định dạng tên <MSSV>.rar, bên trong có thư mục <MSSV> chứa PathNode.h, PathNode.cpp, Algo.h, Algo.cpp, main.cpp và <MSSV>.pdf. Nộp file trên LMS vào ngày cuối 06/12/2026, khi cổng nộp file mở.
- Kiểm tra — Giải nén thử, so tên và cấu trúc, xác nhận cả năm bài code và file RAR đã được hệ thống ghi nhận trước hạn.
- Bàn giao — Hai phần nộp hoàn chỉnh và xác nhận nộp thành công.

## 7. Điều kiện tích hợp, báo cáo và nộp

- **BASE.1–BASE.6:** bảo toàn chữ ký công khai, giữ danh sách đúng thứ tự, xử lý input rỗng và không đường. Kiểm thử relax thật sự đổi parent, bản ghi heap cũ hoặc cập nhật vị trí heap, và truy vết trên chuỗi nhiều bước.
- **BASE.7:** một ví dụ nhỏ cho mỗi API, in P_safe và ma trận PCB ở demo nếu áp dụng. Giải phóng mọi list. Demo cục bộ và test riêng tránh xung đột main.
- **QA.1:** kiểm thử hồi quy cho cả năm API, cả start==goal và không đường nếu phù hợp. Đối chiếu h=0 cho dữ liệu chi phí không âm, xác nhận tính hợp lệ từng cạnh và tự tính lại chi phí. Có thể chạy sanitizer nếu môi trường hỗ trợ. So số mở rộng trên cùng định nghĩa và cùng dữ liệu.
- **QA.2:** xuất năm snippet độc lập. Nếu template LMS chưa có, cung cấp bản đầy đủ và cách dùng bản bỏ struct khi template đã cung cấp, hoặc hai bản nếu cần để copy nguyên file. Build từng snippet bằng harness cục bộ đúng thứ tự ghép. Chỉ ghi đã pass LMS khi có log hoặc ảnh xác nhận từ lần chạy thật. Đối chiếu tie-break, nhãn và P_safe với bằng chứng.
- **DOC.1:** cung cấp dàn ý hoặc nguồn LaTeX/Markdown đầy đủ theo dữ liệu tôi đưa, có đủ năm yêu cầu báo cáo trong mục 2. Gắn [CẦN CHẠY] ở chỗ thiếu thực nghiệm, phân biệt trace tay và trace chương trình, không điền MSSV hay số liệu giả. Báo cáo cuối có tên <MSSV>.pdf.
- **SUB.1:** hai phần nộp theo §4.2. Nộp từng API vào đúng ô CodeRunner. Thư mục <MSSV> chứa PathNode.h, PathNode.cpp, Algo.h, Algo.cpp, main.cpp, <MSSV>.pdf. Tạo RAR thật tên <MSSV>.rar, giải nén thử và build lại từ bản giải nén. Đề bản 04/11 ghi hạn 06/12/2026 và cổng nộp file mở ngày cuối. Tách tình trạng đã chuẩn bị, đã nộp và đã có xác nhận LMS. Ghi các lỗi đóng gói bị trừ 1 điểm mỗi lần theo §4.2. Kiểm tra FAQ/Questions Response Sheet trước khi đặt câu hỏi chính thức.

# CẬP NHẬT TIẾN ĐỘ NHIỆM VỤ

Đây là mục cuối của mọi câu trả lời thực hiện task. Viết thành bản bàn giao độc lập để tôi dán sang phiên sau. Ghi chi tiết theo dữ liệu thực tế, không để các dòng mẫu chưa điền.

1. **Mã, phạm vi và trạng thái** — Hoàn thành / Đã bàn giao, chờ chạy / Một phần / Bị chặn. Chỉ đánh dấu Hoàn thành khi đủ tiêu chí kiểm tra của nhiệm vụ. Tách xác minh local và LMS.
2. **Đã làm cụ thể** — Ghi từng đường dẫn file, hàm/struct/chữ ký, dữ liệu vào/ra, ai gọi hàm, invariant, thuật toán và complexity có điều kiện. Ghi file nào tôi cần dán và chạy lệnh nào để tái lập.
3. **Quyết định và lý do** — Với từng quyết định, ghi vấn đề, phương án chọn, lý do, ảnh hưởng lên tính đúng/complexity/tích hợp và điều kiện đổi. Nếu có phương án khác đã cân nhắc, ghi lý do loại.
4. **Quy ước cần thống nhất và sổ tích lũy** — Nêu các mục mới/đổi, lý do và ca hồi quy cần chạy. Sau đó chép lại toàn bộ sổ từ C01 trở đi ở phiên bản mới nhất, kể cả mục giữ nguyên. Mỗi mục có ID, nhãn, nội dung, căn cứ và nơi áp dụng. Không biến [CHỜ] thành yêu cầu đề nếu chưa có chứng cứ.
5. **Kiểm thử và bằng chứng** — Lệnh, input, kỳ vọng, kết quả quan sát, exit code/log nếu có. Tách ĐÃ CHẠY THẬT, TÍNH TAY và CHƯA CHẠY. Ghi rõ máy/runner nếu có ý nghĩa.
6. **Việc còn mở** — Lỗi, giới hạn, phụ thuộc, câu cần xác minh trên LMS/Q&A, file hoặc bằng chứng cần cung cấp. Liệt kê tiêu chí nào chưa nghiệm thu.
7. **Tự kiểm vấn đáp** — Ba câu hỏi ngắn và đáp án để tôi giải thích được thuật toán, quyết định và một ca biên.
8. **Bàn giao cho nhiệm vụ sau** — Mã tiếp theo có thể làm, danh sách file và log cần mang theo. Giữ bảng trạng thái tích lũy theo mã, cập nhật đúng nhiệm vụ có bằng chứng mới.

# Báo cáo BASE.1 — Khai báo cấu trúc PathNode

**Trạng thái:** Hoàn thành, kiểm thử cục bộ đạt.  
**Phạm vi:** Hàng 48 của sheet `PROGRESS`, đối chiếu §2.3 đề CO1007.  
**Đầu ra:** `PathNode.h` tại thư mục gốc dự án.

## Công việc đã thực hiện

Tạo `PathNode.h` để cả năm bài A* dùng chung kiểu dữ liệu biểu diễn một bước trên đường đi. Khai báo đủ năm trường theo đúng thứ tự trong đề.

| Trường | Kiểu | Vai trò |
|---|---|---|
| `name` | `std::string` | Nhãn của nút hoặc bước đi. |
| `f` | `double` | Tổng chi phí ước lượng qua nút, bằng `g + h`. |
| `g` | `double` | Chi phí thực tế tích lũy từ điểm bắt đầu đến nút. |
| `h` | `double` | Chi phí ước lượng từ nút đến đích. |
| `next` | `PathNode*` | Con trỏ tới nút kế tiếp, bằng `nullptr` ở cuối danh sách. |

Thêm `#include <string>` để header tự cung cấp khai báo cần thiết cho `std::string`. Dùng include guard `CO1007_PATH_NODE_H` để tránh định nghĩa lặp khi header được include nhiều lần trong cùng một đơn vị biên dịch. Khai báo `struct` chỉ mô tả kiểu dữ liệu. Việc cấp phát nút, tính `f` và liên kết các nút thuộc phần dựng danh sách.

## Kiểm thử đã thực hiện

Tạo chương trình kiểm thử tạm, include `PathNode.h` hai lần và dùng hàm `printPath()` mẫu của đề. Khởi tạo hai nút `Start → Goal`, trong đó `Start.next` trỏ đến `Goal` và `Goal.next` bằng `nullptr`.

Biên dịch chương trình kiểm thử bằng GCC với các cờ `-std=c++11 -Wall -Wextra -Werror -pedantic -I.`. Build, chạy chương trình và so sánh stdout với kết quả kỳ vọng đều trả mã thoát `0`.

Kết quả quan sát thực tế:

```text
Solution Path:
Node: Start | f: 3 | g: 0 | h: 3
Node: Goal | f: 4 | g: 4 | h: 0
```

Hai dòng xác nhận `printPath()` đọc đúng `name`, `f`, `g` và `h`. Hàm dùng `next` để chuyển từ `Start` sang `Goal`, rồi dừng ở `nullptr`. Kết quả trên được xác minh cục bộ. LMS CodeRunner chưa được chạy.

## Bàn giao cho BASE.2

Dùng `PathNode.h` làm đầu vào để viết phần dựng và giải phóng linked list. Khi nghiệm thu BASE.2, kiểm tra ba ca gồm danh sách rỗng, một nút và nhiều nút.

# Báo cáo BASE.2 — Dựng và giải phóng linked list

**Phạm vi:** Hàng 49, sheet PROGRESS. Cấu trúc kết quả theo §2.3 đề CO1007.  
**Trạng thái:** Hoàn thành kiểm thử chức năng cục bộ. CodeRunner chưa chạy.

## Đã triển khai

- `PathNode.h` giữ năm trường công khai của `PathNode` và bổ sung `PathStep{name, g, h}` cùng khai báo hai hàm.
- `buildPathList(const std::vector<PathStep>& steps)` nhận dữ liệu theo thứ tự start → goal. Duyệt vector ngược và chèn đầu để giữ thứ tự đó. Tính `f = g + h` tại mỗi nút. Đầu vào rỗng trả `nullptr`. Nếu tạo nút thất bại, hàm giải phóng phần đã tạo rồi truyền tiếp ngoại lệ.
- `freePath(PathNode* head) noexcept` lưu nút kế tiếp trước khi xóa nút hiện tại. Bên gọi sở hữu danh sách trả về và gọi `freePath(head)` một lần khi dùng xong. Nếu còn giữ biến con trỏ, gán lại bằng `nullptr` trước khi tái sử dụng.
- `test_base2.cpp` là chương trình kiểm thử riêng có `main()`, dùng cục bộ.

## Trace tính tay

| Bước | Thao tác | Danh sách sau bước |
|---|---|---|
| 1 | Chèn Goal, g=4, h=0, f=4 | Goal → nullptr |
| 2 | Chèn Mid, g=2, h=1.5, f=3.5 | Mid → Goal → nullptr |
| 3 | Chèn Start, g=0, h=3, f=3 | Start → Mid → Goal → nullptr |

Với L nút và tổng độ dài nhãn B, dựng danh sách mất O(L+B) thời gian và tạo O(L+B) bộ nhớ đầu ra. Hai hàm dùng O(1) bộ nhớ phụ ngoài đầu vào và danh sách. Giải phóng duyệt L nút, mỗi nút đúng một lần nếu đầu vào là danh sách kết thúc bằng nullptr.

## Kiểm thử thực tế

Chạy từ thư mục chứa ba file:

```bash
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -g -fsanitize=address,undefined -fno-omit-frame-pointer -no-pie PathNode.cpp test_base2.cpp -o /tmp/co1007_base2
ASAN_OPTIONS=detect_leaks=0 /tmp/co1007_base2
```

```text
Empty: PASS
Single: PASS
Multiple: PASS
```

Chương trình thoát mã 0. Các ca kiểm tra tên, g/h/f, thứ tự, đuôi nullptr và gọi freePath sau mỗi lần dựng. ASan/UBSan không báo lỗi trong lần chạy này. Lần bật LeakSanitizer bị chặn bởi môi trường ptrace, nên kiểm tra rò rỉ tự động chưa có kết quả.

## Bàn giao

BASE.1 và BASE.2 đã hoàn thành ở phạm vi local. BASE.3 dùng header hiện tại để khai báo năm API trong Algo.h theo §2.2. Mọi quy ước LMS đang chờ xác minh giữ nguyên.

# Báo cáo BASE.3 — Khai báo năm API trong Algo.h

**Phạm vi:** Hàng 50, sheet PROGRESS và §2.2 đề CO1007.  
**Trạng thái:** Hoàn thành khai báo và kiểm thử giao diện cục bộ.

## Công việc đã thực hiện

Tạo `Algo.h`, thêm include guard `CO1007_ALGO_H` và include `PathNode.h` của BASE.2. Khai báo năm hàm ở phạm vi toàn cục theo đúng thứ tự tham số của đề. Tất cả trả `PathNode*`, con trỏ đến đầu danh sách kết quả.

| API | Điểm đã đối chiếu |
|---|---|
| `findMeshPath` | Ma trận trọng số double, sau đó là chỉ số bắt đầu và đích. |
| `findAUVPath` | Ma trận trọng số và tọa độ int có ba cột, rồi start, goal và mode. |
| `findProbabilisticPath` | Hai ma trận double, tọa độ int có hai cột, rồi start, goal, lambda kiểu double và mode. |
| `findGamePath` | Lưới int, kích thước, tọa độ bắt đầu, tọa độ đích và mode. |
| `findPCBPath` | Lưới int, kích thước, tọa độ bắt đầu, tọa độ lối ra, ma trận trọng số đầu ra và mode. |

C++ điều chỉnh tham số mảng hai chiều thành con trỏ tới hàng. Vì vậy kích thước chiều thứ hai như 3 ở AUV và 2 ở Probabilistic là một phần của kiểu tham số.

## Kiểm thử thực tế

Đã dùng GCC với C++11 và các cờ `-Wall -Wextra -Werror -pedantic` để biên dịch một file tạm include `Algo.h` hai lần và gọi đủ năm hàm. Đã liên kết file đó với `PathNode.cpp` và năm định nghĩa giả trong một đơn vị biên dịch riêng. Chữ ký của các định nghĩa giả được trích từ §2.2, mỗi hàm trả nullptr. Cờ `-Wno-unused-parameter` chỉ cần ở bước build có định nghĩa giả. Chương trình chạy và thoát mã 0, console không in gì.

Kết quả xác minh các lời gọi biên dịch và liên kết đúng giao diện. Thuật toán và CodeRunner chưa chạy.

## Tái kiểm tra header trong PowerShell

Chạy từ thư mục chứa Algo.h và PathNode.h:

```powershell
'#include "Algo.h"' | g++ -std=c++11 -Wall -Wextra -Werror -pedantic -I. -x c++ -fsyntax-only -
$LASTEXITCODE
```

Kỳ vọng mã 0. Lệnh này kiểm tra cú pháp và include, không tạo file exe.

## Bàn giao

Dùng Algo.h ở BASE.4 khi bắt đầu phần triển khai trong Algo.cpp. Giữ nguyên các quy ước C01–C12 từ bản bàn giao trước.

# Báo cáo BASE.4 — Khởi tạo trạng thái tìm kiếm A*

**Phạm vi:** Hàng 51, sheet PROGRESS. Công thức f = g + h theo §1.5 đề CO1007.  
**Trạng thái:** Hoàn thành, kiểm thử cục bộ đạt. LMS chưa chạy.

## Đã làm

- Tạo `Algo.cpp`, đặt các kiểu nội bộ trong namespace ẩn danh.
- `SearchRecord` lưu g, h, parent và reached cho mỗi ID. Khi reached là false, giá trị g/h mặc định chỉ là giá trị khởi tạo. Đọc chúng như chi phí có nghĩa sau khi trạng thái đã được phát hiện.
- `SearchState` nhận số trạng thái dương, ID start hợp lệ và hàm heuristic. Cấp một record cho mỗi ID, gọi heuristic đúng một lần cho start, đánh dấu start đã tới và đưa duy nhất start vào frontier. Start có g=0, parent=-1 và f=h(start). Frontier dùng vector, một phần tử đã thỏa tính chất heap.
- `PCBStateKey::encode` mã hóa `cellId * 5 + incomingDirection`. Bốn slot 0–3 dành cho hướng đến, slot 4 dành cho NONE. Với cell ID 2, hướng 0 có ID 10, hướng 1 có ID 11 và NONE có ID 14. BASE.4 chỉ chốt cách lưu khóa nội bộ. Quy ước hướng và đánh số ô vẫn cần được xác minh khi tích hợp PCB.
- Với S khóa trạng thái và heuristic start tốn T_h, thời gian khởi tạo là O(S + T_h) và bộ nhớ là O(S). Frontier ban đầu chứa đúng một phần tử.

## Trace tính tay

Ví dụ có 4 trạng thái, start ID 2, h(2)=3.5.

| Bước | Trạng thái sau bước |
|---|---|
| Cấp records | Bốn record có reached=false và parent=-1. |
| Khởi tạo start | ID 2 có g=0, h=3.5, reached=true và parent=-1. |
| Đưa vào frontier | Frontier chứa một mục (stateId=2, f=3.5). ID 0, 1 và 3 vẫn chưa tới. |

## Kiểm thử thực tế

Đã biên dịch riêng Algo.cpp và chạy test_base4.cpp với GCC C++11 cùng các cờ -Wall -Wextra -Werror -pedantic. Cả hai lệnh build và chương trình test thoát mã 0.

```text
Initial state: PASS
PCB state keys: PASS
```

Ca đầu kiểm tra giá trị start, lời gọi heuristic, frontier và các record chưa tới. Ca PCB kiểm tra khóa theo hướng đến và trạng thái NONE khi khởi tạo.

## Cấu trúc dự án đã chốt

| Vị trí trong Assignment | Nội dung |
|---|---|
| `src/` | `Algo.cpp`, `Algo.h`, `PathNode.cpp`, `PathNode.h`. |
| `tests/` | `test_base2.cpp` và `test_base4.cpp`. |
| `build/` | `co1007_base2.exe` và `co1007_base4.exe`. |
| Thư mục gốc | PDF đề bài và `Prompt_CO1007_AStar.md`, theo ảnh VS Code. |

## Chạy lại trong PowerShell

Từ thư mục gốc dự án:

```powershell
Set-Location 'D:\CTRR\Assignment'
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -I.\src .\tests\test_base4.cpp -o .\build\co1007_base4.exe
if ($LASTEXITCODE -eq 0) { .\build\co1007_base4.exe }
```

`-I.\src` giúp test tìm `Algo.cpp` mà nó include trực tiếp. Test có `main()` riêng và chỉ dùng cục bộ. Ảnh terminal hiển thị `PCB state keys: PASS` trước khi di chuyển file. Lệnh theo bố cục mới là lệnh tái kiểm tra, chưa có kết quả chạy trên máy người dùng.

## Bàn giao

BASE.1–BASE.4 đã hoàn thành ở phạm vi cục bộ. BASE.5 tiếp tục với thao tác heap, chọn trạng thái có f nhỏ nhất, sinh ứng viên và relax. Giữ quy ước C01–C12, bổ sung vào C03 cách khởi tạo frontier và vào C11 mã hóa khóa PCB nội bộ. Quy tắc đồng hạng và các quy ước LMS tiếp tục ở trạng thái chờ.

# Báo cáo BASE.5 — Chọn, mở rộng và cập nhật trạng thái A*

**Phạm vi:** Hàng 52, sheet PROGRESS. **Trạng thái:** Hoàn thành kiểm thử cục bộ. LMS chưa chạy.

## Đã làm

Cập nhật `src/Algo.cpp` với vòng lặp nội bộ `searchUntilGoal`. Hàm lấy trạng thái có f nhỏ nhất từ heap vector, bỏ mục cũ nếu g lưu trong heap khác g tốt nhất và kiểm tra đích trước khi mở rộng. Callback sinh các bước hợp lệ cho từng bài và gọi emit với ID cùng chi phí cạnh. Khi trạng thái chưa được phát hiện hoặc g ứng viên nhỏ hơn g tốt nhất, vòng lặp cập nhật g, h, parent và đưa mục mới vào heap. Trạng thái từng mở rộng có thể được đưa trở lại heap. h được tính lần đầu cho mỗi trạng thái.

Đồng hạng dùng f, rồi h, rồi ID tăng dần. Đây là quy ước C04 tạm thời, chờ testcase LMS. C03 dùng lazy deletion và so sánh g của bản ghi. Hợp đồng nội bộ yêu cầu ID hợp lệ, chi phí cạnh hữu hạn không âm, g/h/f hữu hạn và heuristic cố định trong mỗi lần chạy.

## Trace ca relax và mở lại

| Thao tác | Kết quả |
|---|---|
| Mở rộng 0 | Ghi g(1)=5, parent(1)=0. Ghi g(2)=1, h(2)=5. |
| Lấy 1 với f=5 | Cập nhật g(3) từ 20 xuống 7. |
| Lấy 2 với f=6 | Tìm 0 → 2 → 1 giá 2. Ghi g(1)=2, parent(1)=2 và đưa 1 lại vào heap. |
| Mở rộng lại 1 | Cập nhật g(3) từ 7 xuống 4. Các mục cũ của 3 bị bỏ qua khi pop. |

Ca đồng hạng đưa ba mục có f=3 vào heap. ID 1 có h=2, ID 2 và 3 có h=1, nên ID 2 được chọn. Thứ tự phát sinh là 3, 1, 2.

## Complexity

Với S trạng thái, H mục heap tối đa, Q thao tác heap và K tổng chi phí sinh/xét hàng xóm, thời gian là O(S + K + Q log(max(2,H)) + tổng chi phí gọi heuristic). Bộ nhớ engine là O(S+H). Heap có thể giữ nhiều mục cho cùng ID. Dừng khi pop goal hợp lệ đầu tiên cần heuristic admissible để bảo đảm tối ưu với cơ chế reopen và h(goal)=0.

## Kiểm thử

GCC C++11 với -Wall -Wextra -Werror -pedantic. Algo.cpp biên dịch riêng thành công. Test BASE.4 hồi quy đạt. BASE.5 trả mã 0 và in:

```text
Relax, reopen, stale entries: PASS
Tie-break: PASS
```

Trên PowerShell, từ D:\CTRR\Assignment:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -I.\src .\tests\test_base5.cpp -o .\build\co1007_base5.exe
if ($LASTEXITCODE -eq 0) { .\build\co1007_base5.exe }
```

Test include Algo.cpp để kiểm tra kiểu nội bộ và có main riêng. <functional> chỉ dùng trong test. Mã bài chỉ thêm <algorithm>.

## Bàn giao

BASE.1–BASE.5 hoàn thành cục bộ. BASE.6 dùng ID đích hoặc -1 từ vòng lặp, truy vết parent và dựng PathNode. C01–C12 giữ nội dung đã chốt, với cập nhật C03/C04 nêu trên. Chờ xác minh quy ước LMS.

# Báo cáo BASE.6 — Kết thúc tìm kiếm và truy vết đường đi

**Phạm vi:** Hàng 53, sheet PROGRESS của CheckList (3).xlsx, mục BASE.6 của prompt và §2.2–2.3 của PDF đề CO1007.  
**Trạng thái:** Hoàn thành phần truy vết dùng chung, kiểm thử cục bộ đạt. Quy ước đầu ra LMS còn chờ xác minh.

## Công việc đã làm

- Bổ sung `incomingCost` vào record trong `src/Algo.cpp`. Mỗi lần relax thành công, cập nhật chi phí cạnh vào cùng với parent.
- `reconstructPath` nhận ID do `searchUntilGoal` trả về. Với ID -1, trả nullptr. Với ID đích hợp lệ, lần theo parent đến -1, rồi duyệt chuỗi theo chiều start → goal để tạo PathStep. Callback nhận ID đầy đủ của trạng thái và parent để mỗi bài tự đặt nhãn, kể cả trạng thái PCB có hướng đến.
- Cộng lại g từ chi phí các cạnh trên chuỗi parent cuối. Sau reopen, g từng lưu ở một nút có thể được tính từ phiên bản cũ của đường đến tổ tiên. Tính lại giúp g khớp tuyến đang trả về. Quyết định này không thay đổi thứ tự tìm kiếm hoặc điều kiện dừng.
- Dùng buildPathList từ BASE.2 để tính f = g + h và nối danh sách. Trường hợp start trùng goal trả một nút có g=h=f=0 theo quy ước C05 hiện tại.
- Giữ nguyên chữ ký năm API. Các bài sẽ gọi phần truy vết khi tích hợp engine.

## Trace tính tay

Với 0 → 2 giá 10, 0 → 1 giá 2 và 1 → 2 giá 3, engine cập nhật parent(2) từ 0 sang 1. Truy vết cho chuỗi 2 → 1 → 0. Khi dựng list, kết quả là:

| ID | g | h | f |
|---|---:|---:|---:|
| 0 | 0 | 3 | 3 |
| 1 | 2 | 1 | 3 |
| 2 | 5 | 0 | 5 |

Với L nút và tổng độ dài nhãn B, truy vết và dựng list mất O(L+B) thời gian nếu chi phí tạo nhãn tỷ lệ với độ dài nhãn. Vector truy vết và bước trung gian dùng O(L+B) bộ nhớ phụ. Danh sách đầu ra dùng O(L+B).

## Kiểm thử thực tế

GCC C++11 với -Wall -Wextra -Werror -pedantic. Đã chạy hồi quy BASE.4 và BASE.5. Ba ca BASE.6 kiểm tra đường nhiều bước, start trùng goal và đích không thể tới. Test đối chiếu thứ tự, nhãn, g/h/f và đuôi danh sách. Tất cả chương trình trả mã thoát 0.

```text
Multi-step path: PASS
Start equals goal: PASS
Unreachable goal: PASS
```

Từ PowerShell tại D:\CTRR\Assignment:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -I.\src .\tests\test_base6.cpp .\src\PathNode.cpp -o .\build\co1007_base6.exe
if ($LASTEXITCODE -eq 0) { .\build\co1007_base6.exe }
```

Test include trực tiếp Algo.cpp và liên kết PathNode.cpp để dùng buildPathList/freePath. File test chỉ dùng cục bộ. Lệnh PowerShell và LMS chưa được chạy.

## Bàn giao

BASE.1–BASE.6 hoàn thành cục bộ. C03 bổ sung truy vết theo parent đầy đủ và tính lại g trên tuyến trả về. C05 tiếp tục dùng nullptr cho ca không có đường và một nút chi phí 0 khi start trùng goal, chờ đối chiếu LMS. C06 để từng bài định nghĩa nhãn qua callback. Các quy ước khác giữ theo bàn giao BASE.5. Tiếp theo là T1.1 theo thứ tự tích hợp của prompt.

# Báo cáo BASE.7 — Demo năm bài A*

**Trạng thái:** Đã bàn giao main.cpp, chờ tích hợp năm API để chạy demo.  
**Phạm vi:** BASE.7, hàng 54 sheet PROGRESS, §2.1–2.3 đề CO1007.

## Đã làm

Chuẩn bị nội dung `src/main.cpp` để copy từ chat. Mỗi ví dụ gọi đúng một API, in name/g/h/f và giải phóng danh sách bằng freePath. Demo PCB in vùng 2 × 2 của ma trận đầu ra. Demo Probabilistic tính xác suất trực tiếp từ dữ liệu mẫu vì chỉ có một tuyến từ start đến goal. Giá trị đó dùng để đối chiếu, không phải trường do API trả về.

| Bài | Dữ liệu chính | Kỳ vọng tính tay |
|---|---|---|
| Mesh | Hai nút nối hai chiều, trọng số 2 | Tuyến 0 → 1, g(goal)=2.1 với bậc 1. |
| AUV | Cạnh giá 10, chênh sâu 100, mode 2 | Tuyến 0 → 1, g(goal)=20. |
| Probabilistic | Cạnh dài 5, rủi ro 0.25, lambda=4, mode 1 | Tuyến 0 → 1, g(goal)=6, P_safe=0.75. |
| Game | Lưới 1 × 2, đích là ô lầy, mode 1 | Sang phải, g(goal)=2. |
| PCB | Lưới trống 1 × 2, mode 1 | Một bước, không phí rẽ, g(goal)=1 và ma trận 0 1 / 1 0 theo C11 tạm thời. |

## Kiểm thử thực tế

GCC C++11 đã biên dịch main.cpp thành object với -Wall -Wextra -Werror -pedantic. Khi liên kết với Algo.cpp và PathNode.cpp hiện tại, linker trả mã 1 và báo thiếu định nghĩa của cả năm API. Các kết quả trong bảng là tính tay. Chưa có output demo hoặc kết quả LMS.

## Bàn giao

Đặt main.cpp trong src. Khi các task T1–T5 hoàn tất, build từ thư mục gốc bằng PowerShell:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic .\src\main.cpp .\src\Algo.cpp .\src\PathNode.cpp -o .\build\co1007_demo.exe
if ($LASTEXITCODE -eq 0) { .\build\co1007_demo.exe }
```

Đối chiếu đường đi, chi phí, xác suất và ma trận với bảng. Nhãn, sentinel, mode Hazard và quy tắc PCB tiếp tục theo các quy ước tạm trong prompt cho tới khi được xác minh. BASE.1–BASE.6 đã hoàn thành cục bộ. BASE.7 chờ chạy sau khi tích hợp.

# T1.1 — Cạnh hoạt động và bậc nút Mesh

**Phạm vi:** Hàng 55, sheet PROGRESS. **Trạng thái:** Kiểm thử cục bộ đạt, quy ước LMS còn chờ xác minh.

## Đã làm

Thử nghiệm hai helper nội bộ trong Algo.cpp. isMeshEdge nhận diện cạnh giữa hai nút khác nhau có độ trễ dương. computeMeshDegrees quét ma trận 100 × 100 và đếm cạnh ra theo từng hàng. Bỏ đường chéo theo quy ước tạm C08, dùng 0 làm ô không cạnh trong dữ liệu local theo C07. Không đặt ngưỡng trên tùy ý. Thời gian O(V²), bộ nhớ O(V) cho bảng bậc.

## Kiểm thử

Dữ liệu: 0→0 giá 9, 0→1 giá 2, 0→2 giá 1e9, 1→2 giá 3, 2→0 giá -1. Các ô khác bằng 0. Nút 2 có cạnh vào nhưng không có cạnh ra. Nút 3 cô lập.

| Nút | Cạnh ra được tính | Bậc tính tay | Bậc chạy thật |
|---|---|---:|---:|
| 0 | 0→1, 0→2 | 2 | 2 |
| 1 | 1→2 | 1 | 1 |
| 2 | Không có | 0 | 0 |
| 3 | Không có | 0 | 0 |

GCC C++11 biên dịch và chạy harness tạm với -Wall -Wextra -Werror -pedantic, mã thoát 0. Harness đối chiếu toàn bộ 100 bậc. Chưa chạy PowerShell trên máy người dùng hoặc LMS.

## Bàn giao

Code ASCII được cung cấp trong chat để tích hợp vào src/Algo.cpp. T1.2 dùng bảng bậc để tính phí khi đi vào nút đích của cạnh và xây dựng heuristic. BASE.7 vẫn chờ tích hợp năm API.

# Báo cáo BASE.7 — Demo năm bài A*

**Trạng thái:** Đã bàn giao main.cpp, chờ tích hợp năm API để chạy demo.  
**Phạm vi:** BASE.7, hàng 54 sheet PROGRESS, §2.1–2.3 đề CO1007.

## Đã làm

Chuẩn bị nội dung `src/main.cpp` để copy từ chat. Mỗi ví dụ gọi đúng một API, in name/g/h/f và giải phóng danh sách bằng freePath. Demo PCB in vùng 2 × 2 của ma trận đầu ra. Demo Probabilistic tính xác suất trực tiếp từ dữ liệu mẫu vì chỉ có một tuyến từ start đến goal. Giá trị đó dùng để đối chiếu, không phải trường do API trả về.

| Bài | Dữ liệu chính | Kỳ vọng tính tay |
|---|---|---|
| Mesh | Hai nút nối hai chiều, trọng số 2 | Tuyến 0 → 1, g(goal)=2.1 với bậc 1. |
| AUV | Cạnh giá 10, chênh sâu 100, mode 2 | Tuyến 0 → 1, g(goal)=20. |
| Probabilistic | Cạnh dài 5, rủi ro 0.25, lambda=4, mode 1 | Tuyến 0 → 1, g(goal)=6, P_safe=0.75. |
| Game | Lưới 1 × 2, đích là ô lầy, mode 1 | Sang phải, g(goal)=2. |
| PCB | Lưới trống 1 × 2, mode 1 | Một bước, không phí rẽ, g(goal)=1 và ma trận 0 1 / 1 0 theo C11 tạm thời. |

## Kiểm thử thực tế

GCC C++11 đã biên dịch main.cpp thành object với -Wall -Wextra -Werror -pedantic. Khi liên kết với Algo.cpp và PathNode.cpp hiện tại, linker trả mã 1 và báo thiếu định nghĩa của cả năm API. Các kết quả trong bảng là tính tay. Chưa có output demo hoặc kết quả LMS.

## Bàn giao

Đặt main.cpp trong src. Khi các task T1–T5 hoàn tất, build từ thư mục gốc bằng PowerShell:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic .\src\main.cpp .\src\Algo.cpp .\src\PathNode.cpp -o .\build\co1007_demo.exe
if ($LASTEXITCODE -eq 0) { .\build\co1007_demo.exe }
```

Đối chiếu đường đi, chi phí, xác suất và ma trận với bảng. Nhãn, sentinel, mode Hazard và quy tắc PCB tiếp tục theo các quy ước tạm trong prompt cho tới khi được xác minh. BASE.1–BASE.6 đã hoàn thành cục bộ. BASE.7 chờ chạy sau khi tích hợp.


# T1.2 — Phí tắc nghẽn và heuristic Mesh

**Phạm vi:** Hàng 56, sheet PROGRESS, mục Mesh trong §2.1 đề CO1007.  
**Trạng thái:** Kiểm thử cục bộ đạt. API Mesh sẽ tích hợp ở T1.3.

## Đã làm

- Tính chi phí cạnh hợp lệ bằng trọng số cơ sở cộng 0.1 nhân bậc ra của nút đến, theo quy ước tạm C08 từ T1.1.
- Chọn cận B là chi phí cạnh hợp lệ nhỏ nhất đi vào goal, hoặc 0 khi không có cạnh vào. Dùng h(goal)=0 và h(n)=B cho các nút còn lại.
- Mọi tuyến hữu hạn từ nút khác goal đều có cạnh cuối đi vào goal, giá ít nhất B. Các cạnh trước có chi phí không âm, nên h là cận dưới. Với cạnh chưa vào goal, hai giá trị h bằng nhau hoặc h ở nguồn là 0. Với cạnh vào goal, chi phí cạnh ít nhất B. Vì vậy h nhất quán.
- Tính bảng bậc O(V²), quét cột goal O(V), tính chi phí cạnh hoặc tra h O(1).

## Kiểm thử cục bộ

Đồ thị mẫu có các cạnh 0→1 và 0→2 giá 4, 1→3 giá 2, 2→3 giá 1 và 2→4 giá 9. Bậc của 1 và 2 lần lượt là 1 và 2. Chi phí vào hai nút là 4.1 và 4.2. Goal 3 có bậc 0, nên hai cạnh vào goal giá 2 và 1. Cận B bằng 1, h(3)=0.

Harness tạm dùng vòng lặp BASE.5, kiểm tra cả đích không có cạnh vào và so sánh với h=0. Kết quả thật từ GCC C++11, mã thoát 0:

```text
4.1 4.2
1 0
5.2 5.2
```

Hai lượt đều cho chi phí đích 5.2, phù hợp tuyến 0→2→3 tính tay. Chưa chạy Windows hoặc LMS.

## Bàn giao

Dán các helper ASCII từ chat vào src/Algo.cpp sau phần T1.1. T1.3 sẽ gọi computeMeshDegrees một lần, tính B một lần và truyền heuristic cùng hàm sinh cạnh vào engine. C07/C08 vẫn chờ xác minh theo LMS.


# Báo cáo T1.3 — Tích hợp findMeshPath

**Phạm vi:** Hàng 57, sheet PROGRESS, §2.1–2.3 đề CO1007.  
**Trạng thái:** Hoàn thành, kiểm thử cục bộ đạt.

## Công việc đã làm

Tích hợp các helper Mesh của T1.1–T1.2 với phần A* và truy vết BASE.4–BASE.6. Kiểm tra chỉ số đầu vào trước khi đọc ma trận. Tính bảng bậc và cận dưới một lần. MeshNeighbors quét hàng của nút hiện tại và phát các cạnh hợp lệ cùng chi phí. Dùng functor có toán tử template để tương thích C++11. API nằm ở phạm vi toàn cục. Bên gọi giải phóng kết quả bằng freePath.

Nhãn thử là ID thập phân. Với ca không đường hoặc đầu vào không hợp lệ, trả nullptr. Đây là quy ước local đang chờ đối chiếu CodeRunner cùng cách đếm bậc, sentinel và đồng hạng.

## Trace tính tay và kết quả kiểm thử

Mẫu có các cạnh 0→1 và 0→2 giá 4, 1→3 giá 2, 2→3 giá 1 và 2→4 giá 9. Sau khi mở rộng 0, nút 1 có g=4.1, nút 2 có g=4.2. Mở rộng 1 cho g(3)=6.1. Mở rộng 2 cập nhật g(3)=5.2 và parent(3)=2.

| Nút | g | h | f |
|---|---:|---:|---:|
| 0 | 0 | 1 | 1 |
| 2 | 4.2 | 1 | 5.2 |
| 3 | 5.2 | 0 | 5.2 |

GCC C++11 biên dịch, liên kết API thật với PathNode.cpp và chạy harness với -Wall -Wextra -Werror -pedantic. Mã thoát 0. Ca chính kiểm tra đủ nhãn, cạnh, g/h/f và đuôi danh sách. Ca biên kiểm tra start trùng goal, đích không tới được và chỉ số ngoài phạm vi. Windows và LMS chưa chạy.

## Độ phức tạp và bàn giao

Tính bậc O(V²), tính cận O(V). Với heuristic nhất quán và chi phí hữu hạn không âm, phần tìm kiếm quét các hàng ma trận và dùng heap lazy deletion, tổng thời gian O(V² + E log V) với V ≥ 2. Bộ nhớ engine O(V+E), chưa tính ma trận đầu vào và dữ liệu truy vết. T1.4 tiếp tục với ca phí tắc nghẽn làm đổi tuyến ưu tiên. BASE.7 đã có API Mesh, còn chờ AUV, Probabilistic, Game và PCB.

# T1.4 — Kiểm thử và truy vết phí tắc nghẽn Mesh

**Phạm vi:** Hàng 58, sheet PROGRESS. **Trạng thái:** Kiểm thử cục bộ đạt.

## Dữ liệu và tính tay

Các cạnh có hướng là 0→1 giá 0.8, 1→3 giá 1.2, 0→2 giá 1.15, 2→3 giá 1.0, 1→4 và 1→5 cùng giá 9. Các ô khác bằng 0. Start là 0, goal là 3. Bậc ra của nút 0 đến 5 lần lượt là 2, 3, 1, 0, 0, 0. Hai cạnh phụ kết thúc tại nút cụt và làm tăng bậc nút 1.

| Tuyến | Độ trễ cơ sở | Phí tắc nghẽn | Tổng |
|---|---:|---:|---:|
| 0→1→3 | 2.00 | 0.30 | 2.30 |
| 0→2→3 | 2.15 | 0.10 | 2.25 |

## Trace tính tay

Cận dưới bằng 1 do cạnh 2→3 có chi phí 1. h(3)=0, các nút khác có h=1.

| Nút được lấy khỏi heap | Cập nhật (nút, g, h, f, parent) |
|---|---|
| 0 | (1, 1.10, 1, 2.10, 0), (2, 1.25, 1, 2.25, 0) |
| 1 | (3, 2.30, 0, 2.30, 1), (4, 10.10, 1, 11.10, 1), (5, 10.10, 1, 11.10, 1) |
| 2 | (3, 2.25, 0, 2.25, 2), thay mục cũ của 3 |
| 3 | Dừng và truy vết 0→2→3 |

## Kết quả chạy thực tế

Biên dịch harness với API Mesh T1.3 và PathNode.cpp bằng GCC C++11, cờ -Wall -Wextra -Werror -pedantic. Kiểm tra nhãn, g/h/f và đuôi danh sách. Chương trình trả mã 0.

```text
0 g=0 h=1 f=1
2 g=1.25 h=1 f=2.25
3 g=2.25 h=0 f=2.25
PASS
```

Trace trên là tính tay. Chương trình kiểm tra đầu ra cuối của API. Cách tính bậc ra giữ theo C08. Windows và LMS chưa được chạy cho T1.4.

## Bàn giao

Dùng dữ liệu, trace và kết quả để viết phần Mesh. Tiếp theo là T2.1. BASE.7 vẫn chờ tích hợp đủ năm API.
