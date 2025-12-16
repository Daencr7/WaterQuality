/* ========== Đồng hồ ========== */
function updateTimeLabel() {
  const label = document.getElementById("lastUpdate");
  const now = new Date();
  const hh = String(now.getHours()).padStart(2, "0");
  const mm = String(now.getMinutes()).padStart(2, "0");
  const ss = String(now.getSeconds()).padStart(2, "0");
  label.textContent = `${hh}:${mm}:${ss}`;
}
updateTimeLabel();
setInterval(updateTimeLabel, 1000);

/* ========== Điều hướng page ========== */
const PAGE_TITLES = {
  overview: "Tổng quan chất lượng",
  history: "Biểu đồ & Lịch sử",
  alerts: "Cảnh báo & Ngưỡng",
  forecast: "Dự báo AI",
};

const navItems = document.querySelectorAll(".nav-item");
const pages = document.querySelectorAll(".page");
const pageLabel = document.getElementById("pageLabel");

// mình sẽ push các chart history/alerts vào array để resize khi đổi tab
const historyCharts = [];
const levelCharts = [];

navItems.forEach((item) => {
  item.addEventListener("click", () => {
    const target = item.getAttribute("data-target");

    navItems.forEach((n) => n.classList.remove("active"));
    item.classList.add("active");

    pages.forEach((p) => p.classList.remove("active"));
    document.getElementById("page-" + target).classList.add("active");

    pageLabel.textContent = PAGE_TITLES[target] || "Giám sát";

    // fix kích thước chart khi vừa hiện tab
    if (target === "history") {
      historyCharts.forEach((c) => c.resize());
    }
    if (target === "alerts") {
      levelCharts.forEach((c) => c.resize());
    }
  });
});

/* ============================================================
   OVERVIEW: CHỈ CÒN BIỂU ĐỒ 4 GIỜ GẦN NHẤT + VÒNG TRÒN CẢNH BÁO
   (đã bỏ bar chart 30 ngày)
============================================================ */

const timeCtx = document.getElementById("timeSeriesChart");
const timeLabels = Array.from({ length: 10 }, (_, i) => `${i}:00`);

const timeSeriesChart = new Chart(timeCtx, {
  type: "line",
  data: {
    labels: timeLabels,
    datasets: [
      {
        label: "pH",
        data: [7.1, 7.2, 7.3, 7.2, 7.25, 7.22, 7.24, 7.21, 7.22, 7.2],
        borderColor: "#4ade80",
        backgroundColor: "transparent",
        borderWidth: 2,
        tension: 0.35,
      },
      {
        label: "TDS (mg/L)",
        data: [780, 790, 810, 825, 830, 835, 828, 820, 815, 820],
        borderColor: "#60a5fa",
        backgroundColor: "transparent",
        borderWidth: 2,
        tension: 0.35,
      },
      {
        label: "Độ đục (NTU)",
        data: [6.5, 6.8, 7.1, 7.3, 7.0, 7.2, 7.3, 7.4, 7.3, 7.4],
        borderColor: "#f87171",
        backgroundColor: "transparent",
        borderWidth: 2,
        tension: 0.35,
      },
      {
        label: "Nhiệt độ (°C)",
        data: [28.9, 29.0, 29.2, 29.3, 29.4, 29.5, 29.6, 29.5, 29.4, 29.5],
        borderColor: "#fbbf24",
        backgroundColor: "transparent",
        borderWidth: 2,
        tension: 0.35,
      },
    ],
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
    interaction: { mode: "index", intersect: false },
    scales: {
      x: {
        grid: { display: false },
        ticks: { color: "#9ca3af", font: { size: 11 } },
      },
      y: {
        grid: { color: "#1f2937" },
        ticks: { color: "#9ca3af", font: { size: 11 } },
      },
    },
    plugins: {
      legend: {
        labels: { color: "#e5e7eb", font: { size: 11 } },
      },
    },
  },
});

/* ============================================================
   HISTORY PAGE: 4 biểu đồ riêng (pH, Temp, TDS, NTU)
============================================================ */

const historyLabels = Array.from({ length: 24 }, (_, i) => `${i}:00`);

function makeHistoryDataset(base, jitterAmount) {
  return historyLabels.map(
    () => base + (Math.random() * 2 - 1) * jitterAmount
  );
}

// 1. pH
historyCharts.push(
  new Chart(document.getElementById("histPhChart"), {
    type: "line",
    data: {
      labels: historyLabels,
      datasets: [
        {
          label: "pH",
          data: makeHistoryDataset(7.2, 0.2),
          borderColor: "#4ade80",
          backgroundColor: "transparent",
          borderWidth: 2,
          tension: 0.3,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
        y: {
          grid: { color: "#1f2937" },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
      },
      plugins: {
        legend: { labels: { color: "#e5e7eb", font: { size: 11 } } },
      },
    },
  })
);

// 2. Nhiệt độ
historyCharts.push(
  new Chart(document.getElementById("histTempChart"), {
    type: "line",
    data: {
      labels: historyLabels,
      datasets: [
        {
          label: "Nhiệt độ (°C)",
          data: makeHistoryDataset(29.5, 0.4),
          borderColor: "#fbbf24",
          backgroundColor: "transparent",
          borderWidth: 2,
          tension: 0.3,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
        y: {
          grid: { color: "#1f2937" },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
      },
      plugins: {
        legend: { labels: { color: "#e5e7eb", font: { size: 11 } } },
      },
    },
  })
);

// 3. TDS
historyCharts.push(
  new Chart(document.getElementById("histTdsChart"), {
    type: "line",
    data: {
      labels: historyLabels,
      datasets: [
        {
          label: "TDS (mg/L)",
          data: makeHistoryDataset(820, 30),
          borderColor: "#60a5fa",
          backgroundColor: "transparent",
          borderWidth: 2,
          tension: 0.3,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
        y: {
          grid: { color: "#1f2937" },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
      },
      plugins: {
        legend: { labels: { color: "#e5e7eb", font: { size: 11 } } },
      },
    },
  })
);

// 4. NTU
historyCharts.push(
  new Chart(document.getElementById("histNtuChart"), {
    type: "line",
    data: {
      labels: historyLabels,
      datasets: [
        {
          label: "Độ đục (NTU)",
          data: makeHistoryDataset(7, 0.8),
          borderColor: "#f87171",
          backgroundColor: "transparent",
          borderWidth: 2,
          tension: 0.3,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
        y: {
          grid: { color: "#1f2937" },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
      },
      plugins: {
        legend: { labels: { color: "#e5e7eb", font: { size: 11 } } },
      },
    },
  })
);

/* ============================================================
   ALERTS PAGE: 3 biểu đồ mức 1/2/3
============================================================ */
const weekLabels = ["Tuần 4", "Tuần 3", "Tuần 2", "Tuần này"];

function makeLevelChart(canvasId, color, data) {
  const chart = new Chart(document.getElementById(canvasId), {
    type: "bar",
    data: {
      labels: weekLabels,
      datasets: [
        {
          label: "Số cảnh báo",
          data,
          backgroundColor: color,
          borderRadius: 6,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
        y: {
          beginAtZero: true,
          grid: { color: "#1f2937" },
          ticks: { color: "#9ca3af", font: { size: 11 } },
        },
      },
      plugins: { legend: { display: false } },
    },
  });
  levelCharts.push(chart);
}

makeLevelChart("level1Chart", "#22c55e", [5, 6, 4, 3]);
makeLevelChart("level2Chart", "#f97316", [2, 3, 2, 1]);
makeLevelChart("level3Chart", "#ef4444", [1, 1, 1, 0]);

/* ============================================================
   VÒNG TRÒN CẢNH BÁO – LOGIC MỨC 0/1/2/3
   Dùng ngưỡng giống bảng cảnh báo ở trang Cảnh báo & Ngưỡng
============================================================ */

function calculateAlertLevel(current) {
  const ph = current.ph;
  const tds = current.tds;
  const ntu = current.ntu;
  const temp = current.temp;

  let level = 0;

  // MỨC 3
  if (
    ph < 4.2 || ph > 11.1 ||
    tds > 1950 ||
    ntu > 130 ||
    temp > 52
  ) {
    level = 3;
  }
  // MỨC 2
  else if (
    ph < 4.8 || ph > 10.2 ||
    tds > 1800 ||
    ntu > 120 ||
    temp > 48
  ) {
    level = 2;
  }
  // MỨC 1
  else if (
    ph < 5.4 || ph > 9.35 ||
    tds > 1650 ||
    ntu > 110 ||
    temp > 44
  ) {
    level = 1;
  } else {
    level = 0;
  }

  return level;
}

function updateAlertCircle(level) {
  const circle = document.getElementById("alertCircle");
  const text = document.getElementById("alertLevelText");

  // reset class
  circle.className = "alert-circle";

  // thêm class theo mức
  if (level === 0) circle.classList.add("alert-l0");
  else if (level === 1) circle.classList.add("alert-l1");
  else if (level === 2) circle.classList.add("alert-l2");
  else circle.classList.add("alert-l3");

  text.textContent = `Mức ${level}`;
}

// set mặc định khi mới load
updateAlertCircle(0);

/* ============================================================
   BACKEND: LẤY DỮ LIỆU TỪ API
============================================================ */

async function fetchOverviewFromAPI() {
  try {
    const res = await fetch("api/get_overview.php");
    const json = await res.json();

    if (json.status !== "ok") {
      console.error("get_overview error:", json);
      return;
    }

    // 1. Cập nhật trạng thái hiện tại
    if (json.current) {
      document.getElementById("phValue").textContent   = json.current.ph.toFixed(2);
      document.getElementById("tdsValue").textContent  = Math.round(json.current.tds);
      document.getElementById("ntuValue").textContent  = json.current.ntu.toFixed(1);
      document.getElementById("tempValue").textContent = json.current.temp.toFixed(1);

      // 1b. Cập nhật màu vòng tròn cảnh báo
      let level;
      if (typeof json.current.alert_level === "number") {
        level = json.current.alert_level;
      } else {
        level = calculateAlertLevel(json.current);
      }
      updateAlertCircle(level);
    }

    // 2. Dữ liệu 4 giờ gần nhất -> timeSeriesChart
    if (json.last4h) {
      timeSeriesChart.data.labels           = json.last4h.labels;
      timeSeriesChart.data.datasets[0].data = json.last4h.ph;
      timeSeriesChart.data.datasets[1].data = json.last4h.tds;
      timeSeriesChart.data.datasets[2].data = json.last4h.ntu;
      timeSeriesChart.data.datasets[3].data = json.last4h.temp;
      timeSeriesChart.update("none");
    }

    // 3. Thống kê cảnh báo 30 ngày
    //    → CHỈ dùng cho 3 chart mức 1/2/3 ở tab Cảnh báo
    if (json.alerts30d) {
      if (levelCharts.length === 3) {
        levelCharts.forEach((c) => {
          c.data.labels = json.alerts30d.labels;
        });

        levelCharts[0].data.datasets[0].data = json.alerts30d.level1;
        levelCharts[1].data.datasets[0].data = json.alerts30d.level2;
        levelCharts[2].data.datasets[0].data = json.alerts30d.level3;
        levelCharts.forEach((c) => c.update("none"));
      }
    }
  } catch (err) {
    console.error("fetchOverviewFromAPI error:", err);
  }
}

async function fetchHistoryFromAPI(hours = 24) {
  try {
    // BUGFIX: dùng template string
    const res = await fetch(`api/get_history.php?hours=${hours}`);
    const json = await res.json();

    if (json.status !== "ok") {
      console.error("get_history error:", json);
      return;
    }

    const labels = json.labels || [];

    // 1. pH
    historyCharts[0].data.labels = labels;
    historyCharts[0].data.datasets[0].data = json.ph || [];
    historyCharts[0].update("none");

    // 2. Nhiệt độ
    historyCharts[1].data.labels = labels;
    historyCharts[1].data.datasets[0].data = json.temp || [];
    historyCharts[1].update("none");

    // 3. TDS
    historyCharts[2].data.labels = labels;
    historyCharts[2].data.datasets[0].data = json.tds || [];
    historyCharts[2].update("none");

    // 4. NTU
    historyCharts[3].data.labels = labels;
    historyCharts[3].data.datasets[0].data = json.ntu || [];
    historyCharts[3].update("none");
  } catch (err) {
    console.error("fetchHistoryFromAPI error:", err);
  }
}

// Gọi lần đầu khi load
fetchOverviewFromAPI();
fetchHistoryFromAPI(24);

// Cập nhật lại định kỳ
setInterval(fetchOverviewFromAPI, 10000);          // mỗi 10 giây
setInterval(() => fetchHistoryFromAPI(24), 60000); // mỗi 60 giây
