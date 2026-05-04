const bitNumbersDiv = document.getElementById("bitNumbers");
const registerBitsDiv = document.getElementById("registerBits");
const infoPanel = document.getElementById("infoPanel");
const registerCatalogDiv = document.getElementById("registerCatalog");
const regFilterInput = document.getElementById("regFilter");
const registerMeta = document.getElementById("registerMeta");

const state = {
   registers: [],
   groups: [],
   selectedId: null
};

init();

let currentGroupIndex = 0;
const groupIndexMap = new Map();

async function init() {
   state.registers = await loadAllRegisters();
   if (!state.registers || state.registers.length === 0) {
      showLoadError();
      return;
   }
   state.groups = groupRegisters(state.registers);

   renderCatalog("");
   const first = state.registers.find((item) => item.hasBits) || state.registers[0];
   if (first) {
      selectRegister(first.id);
   }

   regFilterInput.addEventListener("input", (event) => {
      renderCatalog(event.target.value);
   });
}

async function loadAllRegisters() {
   try {
      const response = await fetch("json/registers/index.json");
      if (!response.ok) return [];
      const files = await response.json();
      const promises = files.map(file => fetch(`json/registers/${file}`).then(r => r.json()));
      const registers = await Promise.all(promises);

      let currentGroupIndex = 0;
      const groupIndexMap = new Map();
      registers.forEach(reg => {
         if (!groupIndexMap.has(reg.groupLabel)) {
            groupIndexMap.set(reg.groupLabel, currentGroupIndex++);
         }
         reg.groupIndex = groupIndexMap.get(reg.groupLabel);
      });
      return registers;
   } catch (error) {
      console.error("Failed to load registers:", error);
      return [];
   }
}

function showLoadError() {
   registerCatalogDiv.innerHTML = "";
   const group = document.createElement("div");
   group.className = "catalog-group";
   group.innerHTML = "<h3>Khong tai duoc du lieu</h3><p class=\"reg-meta\">Kiem tra mang, server, hoac file index.json.</p>";
   registerCatalogDiv.appendChild(group);

   registerMeta.innerHTML = "<div class=\"meta-title\">Khong co du lieu thanh ghi</div>";
   bitNumbersDiv.innerHTML = "";
   renderEmptyState("Chua co bang bit de hien thi.");
   infoPanel.innerHTML = "<div class=\"instruction\">Khong the tai du lieu thanh ghi.</div>";
}

function groupRegisters(registers) {
   const groups = new Map();
   registers.forEach((reg) => {
      const key = `${reg.groupIndex}_${reg.groupLabel}`;
      if (!groups.has(key)) {
         groups.set(key, { label: reg.groupLabel, index: reg.groupIndex, registers: [] });
      }
      groups.get(key).registers.push(reg);
   });
   return Array.from(groups.values()).sort((a, b) => a.index - b.index);
}

function renderCatalog(filterText) {
   const keyword = (filterText || "").trim().toLowerCase();
   registerCatalogDiv.innerHTML = "";

   state.groups.forEach((group) => {
      const filtered = group.registers.filter((reg) => {
         return reg.id.toLowerCase().includes(keyword) || (reg.title && reg.title.toLowerCase().includes(keyword));
      });

      if (filtered.length === 0) {
         return;
      }

      const groupEl = document.createElement("div");
      groupEl.className = "catalog-group";
      groupEl.innerHTML = `<h3>${escapeHtml(group.label)}</h3>`;

      const list = document.createElement("div");
      list.className = "catalog-items";

      filtered.forEach((reg, index) => {
         const item = document.createElement("button");
         item.type = "button";
         item.className = "catalog-item";
         if (reg.id === state.selectedId) {
            item.classList.add("active");
         }

         item.style.animationDelay = `${index * 40}ms`;

         const metaParts = [];
         if (reg.offset) metaParts.push(`Offset ${reg.offset}`);
         if (reg.resetValue) metaParts.push(`Reset ${reg.resetValue}`);

         item.innerHTML = `
            <div class="reg-name">${escapeHtml(reg.id)}</div>
            ${reg.title ? `<div class="reg-desc">${escapeHtml(reg.title)}</div>` : ""}
            ${metaParts.length ? `<div class="reg-meta">${escapeHtml(metaParts.join(" | "))}</div>` : ""}
         `;

         item.addEventListener("click", () => selectRegister(reg.id));
         list.appendChild(item);
      });

      groupEl.appendChild(list);
      registerCatalogDiv.appendChild(groupEl);
   });
}

function selectRegister(registerId) {
   const register = state.registers.find((item) => item.id === registerId);
   if (!register) {
      return;
   }
   state.selectedId = registerId;
   renderRegister(register);
   renderCatalog(regFilterInput.value);
}

function renderRegister(register) {
   const metaTags = [];
   if (register.offset) metaTags.push(`Offset: ${register.offset}`);
   if (register.resetValue) metaTags.push(`Reset: ${register.resetValue}`);

   registerMeta.innerHTML = `
      <div style="display: flex; justify-content: space-between; align-items: flex-start;">
         <div>
            <div class="meta-title">${escapeHtml(register.id)}</div>
            ${register.title ? `<div class="meta-sub">${parseSimpleMarkdown(register.title)}</div>` : ""}
            ${metaTags.length ? `<div class="meta-tags">${escapeHtml(metaTags.join(" | "))}</div>` : ""}
         </div>
         <button class="edit-btn" id="openEditBtn">Sửa JSON</button>
      </div>
   `;

   document.getElementById("openEditBtn").addEventListener("click", () => {
      openEditModal(register);
   });

   if (!register.hasBits) {
      bitNumbersDiv.innerHTML = "";
      renderEmptyState("Thanh ghi nay chua co bang bit.");
      infoPanel.innerHTML = "<div class=\"instruction\">Chon thanh ghi khac hoac bo sung bang bit.</div>";
      return;
   }

   renderBitLayout(register);
   infoPanel.innerHTML = "<div class=\"instruction\">Chon bit de xem chi tiet.</div>";
}

function renderBitLayout(register) {
   const bitCount = register.maxBit + 1;
   const useTwoRows = bitCount === 32;
   const rowBitCount = useTwoRows ? 16 : bitCount;
   const minCell = rowBitCount > 24 ? 26 : 32;
   const minWidth = rowBitCount * minCell;

   bitNumbersDiv.style.setProperty("--bit-count", rowBitCount);
   registerBitsDiv.style.setProperty("--bit-count", rowBitCount);
   bitNumbersDiv.style.minWidth = `${minWidth}px`;
   registerBitsDiv.style.minWidth = `${minWidth}px`;

   registerBitsDiv.classList.toggle("dense", rowBitCount > 20);
   registerBitsDiv.classList.remove("empty");
   registerBitsDiv.classList.toggle("two-rows", useTwoRows);
   bitNumbersDiv.classList.toggle("two-rows", useTwoRows);

   bitNumbersDiv.innerHTML = "";
   registerBitsDiv.innerHTML = "";

   function createBitBox(entry, register) {
      const bitBox = document.createElement("button");
      bitBox.type = "button";
      bitBox.className = `bit-box ${entry.isReserved ? "reserved" : ""}`;

      const cleanNameForButton = entry.name.replace(/<br\s*\/?>/gi, " ").replace(/\*\*(.*?)\*\*/g, "$1").replace(/\s*\([^)]*\)/g, "").trim();
      const cleanNameForTitle = entry.name.replace(/<br\s*\/?>/gi, " ").replace(/\*\*(.*?)\*\*/g, "$1").trim();

      bitBox.textContent = cleanNameForButton;
      bitBox.title = `${cleanNameForTitle} [${entry.rangeLabel}]`;

      if (entry.isReserved) {
         bitBox.disabled = true;
      } else {
         bitBox.addEventListener("click", () => {
            document.querySelectorAll(".bit-box").forEach((box) => box.classList.remove("active"));
            bitBox.classList.add("active");
            showInfo(entry, register);
         });
      }

      return bitBox;
   }

   if (useTwoRows) {
      const rows = [
         { start: register.maxBit, end: register.maxBit - 15 },
         { start: register.maxBit - 16, end: 0 }
      ];

      rows.forEach((row) => {
         const numberRow = document.createElement("div");
         numberRow.className = "bit-row";

         const bitRow = document.createElement("div");
         bitRow.className = "bit-row";

         for (let bit = row.start; bit >= row.end; bit -= 1) {
            const numSpan = document.createElement("span");
            numSpan.textContent = bit;
            numSpan.style.textAlign = "center";
            numberRow.appendChild(numSpan);

            const entry = register.bits[bit];
            const bitBox = createBitBox(entry, register);
            bitRow.appendChild(bitBox);
         }

         bitNumbersDiv.appendChild(numberRow);
         registerBitsDiv.appendChild(bitRow);
      });
      return;
   }

   for (let bit = register.maxBit; bit >= 0; bit -= 1) {
      const numSpan = document.createElement("span");
      numSpan.textContent = bit;
      numSpan.style.textAlign = "center";
      bitNumbersDiv.appendChild(numSpan);

      const entry = register.bits[bit];
      const bitBox = createBitBox(entry, register);
      registerBitsDiv.appendChild(bitBox);
   }
}

function renderEmptyState(message) {
   registerBitsDiv.classList.add("empty");
   registerBitsDiv.innerHTML = `<div class=\"empty-state\">${escapeHtml(message)}</div>`;
}

function parseSimpleMarkdown(text) {
   let escaped = escapeHtml(text || "Reserved");
   escaped = escaped.replace(/&lt;br\s*\/?&gt;/gi, "<br>");
   escaped = escaped.replace(/\*\*(.*?)\*\*/g, "<strong>$1</strong>");
   escaped = escaped.replace(/`(.*?)`/g, "<code style='background: #e2e8f0; padding: 2px 4px; border-radius: 4px; font-family: monospace;'>$1</code>");
   return escaped;
}

function showInfo(entry, register) {
   const access = entry.access && entry.access !== "-" ? entry.access : "-";
   infoPanel.innerHTML = `
      <h2>${escapeHtml(register.id)} - Bits [${escapeHtml(entry.rangeLabel)}]</h2>
      <div class="desc-text"><span class="bit-name">${parseSimpleMarkdown(entry.name)}</span></div>
      <div class="desc-text" style="margin-top: 8px; line-height: 1.6;">${parseSimpleMarkdown(entry.desc)}</div>
      <hr style="border: 0; border-top: 1px solid #eee; margin-top: 15px;">
      <p style="color: #7f8c8d; font-size: 14px;"><i>Access: ${escapeHtml(access)}</i></p>
   `;
}

function escapeHtml(value) {
   return String(value)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/\"/g, "&quot;")
      .replace(/'/g, "&#39;");
}

/* ================= MODAL & EDIT LOGIC ================= */

const editModal = document.getElementById('editModal');
const closeEditBtn = document.getElementById('closeEditBtn');
const cancelEditBtn = document.getElementById('cancelEditBtn');
const saveEditBtn = document.getElementById('saveEditBtn');
const jsonEditor = document.getElementById('jsonEditor');

let editingRegisterId = null;

function openEditModal(register) {
   editingRegisterId = register.id;
   const clone = { ...register };
   delete clone.groupIndex;
   jsonEditor.value = JSON.stringify(clone, null, 2);
   editModal.style.display = 'flex';
}

function closeEditModal() {
   editModal.style.display = 'none';
   editingRegisterId = null;
}

closeEditBtn.addEventListener('click', closeEditModal);
cancelEditBtn.addEventListener('click', closeEditModal);

saveEditBtn.addEventListener('click', () => {
   try {
      const parsed = JSON.parse(jsonEditor.value);
      const index = state.registers.findIndex(r => r.id === editingRegisterId);
      if (index !== -1) {
         parsed.groupIndex = state.registers[index].groupIndex;
         state.registers[index] = parsed;
         renderRegister(parsed);
         downloadJSON(parsed.id.toLowerCase() + '.json', JSON.stringify(parsed, null, 2));
         closeEditModal();
      }
   } catch (e) {
      alert('JSON không hợp lệ: ' + e.message);
   }
});

function downloadJSON(filename, text) {
   const element = document.createElement('a');
   element.setAttribute('href', 'data:application/json;charset=utf-8,' + encodeURIComponent(text));
   element.setAttribute('download', filename);
   element.style.display = 'none';
   document.body.appendChild(element);
   element.click();
   document.body.removeChild(element);
}
