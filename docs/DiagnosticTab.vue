<template>
  <div class="diagnostic-container">
    <v-card class="diagnostic-card" elevation="2">
      <v-card-title class="diagnostic-header">
        <v-icon class="mr-2" size="28">mdi-chip</v-icon>
        ESP32 Diagnostic Results
        <v-spacer></v-spacer>
        <v-btn
          color="primary"
          variant="tonal"
          prepend-icon="mdi-refresh"
          @click="loadDiagnostics"
        >
          Refresh
        </v-btn>
      </v-card-title>

      <v-card-subtitle class="text-medium-emphasis">
        View comprehensive hardware analysis and I2C scan results
      </v-card-subtitle>

      <v-divider></v-divider>

      <!-- Loading State -->
      <v-card-text v-if="loading" class="text-center py-8">
        <v-progress-circular
          indeterminate
          color="primary"
          size="64"
        ></v-progress-circular>
        <p class="mt-4">Loading diagnostic data...</p>
      </v-card-text>

      <!-- No Data State -->
      <v-card-text v-else-if="!diagnosticData" class="text-center py-8">
        <v-icon size="64" color="grey-lighten-1">mdi-information-outline</v-icon>
        <h3 class="mt-4">No Diagnostic Data Available</h3>
        <p class="text-medium-emphasis mt-2">
          Flash the diagnostic firmware and capture the output
        </p>
        <v-btn
          color="primary"
          variant="elevated"
          class="mt-4"
          prepend-icon="mdi-upload"
          @click="showUploadDialog = true"
        >
          Upload Diagnostic Data
        </v-btn>
      </v-card-text>

      <!-- Diagnostic Data Display -->
      <v-card-text v-else class="diagnostic-content">
        <!-- Chip Information -->
        <v-expansion-panels class="mb-4">
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon class="mr-2">mdi-memory</v-icon>
              <strong>Chip Information</strong>
              <v-chip class="ml-4" size="small" color="primary">
                {{ diagnosticData.chip.model }}
              </v-chip>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <v-row dense>
                <v-col cols="12" md="6">
                  <v-list density="compact">
                    <v-list-item>
                      <v-list-item-title>Model</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.model }}</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Revision</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.revision }}</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>CPU Cores</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.cores }}</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Frequency</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.frequency_mhz }} MHz</v-list-item-subtitle>
                    </v-list-item>
                  </v-list>
                </v-col>
                <v-col cols="12" md="6">
                  <v-list density="compact">
                    <v-list-item>
                      <v-list-item-title>MAC Address</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.mac_address }}</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Flash Size</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.flash_size_mb }} MB</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Flash Type</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.chip.flash_type }}</v-list-item-subtitle>
                    </v-list-item>
                  </v-list>
                </v-col>
              </v-row>
              
              <!-- Features -->
              <v-divider class="my-4"></v-divider>
              <h4 class="mb-2">Features</h4>
              <v-chip-group>
                <v-chip v-if="diagnosticData.chip.features.wifi" color="success" size="small">
                  <v-icon start>mdi-wifi</v-icon>
                  WiFi
                </v-chip>
                <v-chip v-if="diagnosticData.chip.features.bluetooth_classic" color="info" size="small">
                  <v-icon start>mdi-bluetooth</v-icon>
                  Bluetooth Classic
                </v-chip>
                <v-chip v-if="diagnosticData.chip.features.bluetooth_le" color="info" size="small">
                  <v-icon start>mdi-bluetooth</v-icon>
                  Bluetooth LE
                </v-chip>
                <v-chip v-if="diagnosticData.chip.features.ieee802154" color="warning" size="small">
                  <v-icon start>mdi-zigbee</v-icon>
                  IEEE 802.15.4
                </v-chip>
              </v-chip-group>
            </v-expansion-panel-text>
          </v-expansion-panel>

          <!-- Memory Information -->
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon class="mr-2">mdi-database</v-icon>
              <strong>Memory Information</strong>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <v-row dense>
                <v-col cols="12" md="4">
                  <h4 class="mb-2">Heap Memory</h4>
                  <v-progress-linear
                    :model-value="heapUsagePercent"
                    :color="heapUsagePercent > 80 ? 'error' : 'success'"
                    height="20"
                    class="mb-2"
                  >
                    <template v-slot:default>
                      <strong>{{ heapUsagePercent }}%</strong>
                    </template>
                  </v-progress-linear>
                  <v-list density="compact">
                    <v-list-item>
                      <v-list-item-title>Total</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.heap.total_kb }} KB</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Free</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.heap.free_kb }} KB</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Used</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.heap.used_kb }} KB</v-list-item-subtitle>
                    </v-list-item>
                  </v-list>
                </v-col>
                
                <v-col cols="12" md="4">
                  <h4 class="mb-2">PSRAM</h4>
                  <v-alert
                    v-if="!diagnosticData.memory.psram.available"
                    type="info"
                    density="compact"
                    variant="tonal"
                  >
                    Not Available
                  </v-alert>
                  <v-list v-else density="compact">
                    <v-list-item>
                      <v-list-item-title>Total</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.psram.total_kb }} KB</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Free</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.psram.free_kb }} KB</v-list-item-subtitle>
                    </v-list-item>
                  </v-list>
                </v-col>
                
                <v-col cols="12" md="4">
                  <h4 class="mb-2">Flash Memory</h4>
                  <v-list density="compact">
                    <v-list-item>
                      <v-list-item-title>Size</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.flash.size_mb }} MB</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Speed</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.flash.speed_mhz }} MHz</v-list-item-subtitle>
                    </v-list-item>
                    <v-list-item>
                      <v-list-item-title>Mode</v-list-item-title>
                      <v-list-item-subtitle>{{ diagnosticData.memory.flash.mode }}</v-list-item-subtitle>
                    </v-list-item>
                  </v-list>
                </v-col>
              </v-row>
            </v-expansion-panel-text>
          </v-expansion-panel>

          <!-- GPIO Information -->
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon class="mr-2">mdi-electric-switch</v-icon>
              <strong>GPIO Configuration</strong>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <v-row dense>
                <v-col cols="12" md="6">
                  <h4 class="mb-2">Default I2C Pins</h4>
                  <v-chip color="primary" class="mr-2">
                    SDA: GPIO {{ diagnosticData.gpio.i2c_default.sda }}
                  </v-chip>
                  <v-chip color="primary">
                    SCL: GPIO {{ diagnosticData.gpio.i2c_default.scl }}
                  </v-chip>
                </v-col>
                <v-col cols="12" md="6">
                  <h4 class="mb-2">Pin Summary</h4>
                  <p>Total Pins: {{ diagnosticData.gpio.total_pins }}</p>
                  <p v-if="diagnosticData.gpio.usable_pins">
                    Usable: {{ diagnosticData.gpio.usable_pins.length }} pins
                  </p>
                </v-col>
              </v-row>
              
              <v-alert
                v-if="diagnosticData.gpio.restricted"
                type="warning"
                variant="tonal"
                class="mt-4"
                density="compact"
              >
                <strong>⚠️ Restricted Pins:</strong> GPIO {{ diagnosticData.gpio.restricted.join(', ') }}
                <br><small>These pins are used for SPI Flash or other critical functions</small>
              </v-alert>
            </v-expansion-panel-text>
          </v-expansion-panel>

          <!-- I2C Scan Results -->
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon class="mr-2">mdi-access-point-network</v-icon>
              <strong>I2C Bus Scan</strong>
              <v-chip
                v-if="diagnosticData.i2c_scan.devices.length > 0"
                class="ml-4"
                size="small"
                color="success"
              >
                {{ diagnosticData.i2c_scan.devices.length }} device(s) found
              </v-chip>
              <v-chip v-else class="ml-4" size="small" color="grey">
                No devices
              </v-chip>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <p class="mb-4">
                Scanned on SDA=GPIO{{ diagnosticData.i2c_scan.sda_pin }}, 
                SCL=GPIO{{ diagnosticData.i2c_scan.scl_pin }}
              </p>
              
              <v-alert
                v-if="diagnosticData.i2c_scan.devices.length === 0"
                type="info"
                variant="tonal"
                density="compact"
              >
                No I2C devices detected. Check wiring and pull-up resistors (4.7kΩ recommended).
              </v-alert>
              
              <v-data-table
                v-else
                :headers="i2cHeaders"
                :items="diagnosticData.i2c_scan.devices"
                density="compact"
                class="elevation-1"
              >
                <template v-slot:item.address="{ item }">
                  <v-chip size="small" color="primary">{{ item.address }}</v-chip>
                </template>
                <template v-slot:item.likely="{ item }">
                  <span class="text-caption">{{ item.likely }}</span>
                </template>
              </v-data-table>
            </v-expansion-panel-text>
          </v-expansion-panel>

          <!-- System Information -->
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon class="mr-2">mdi-information</v-icon>
              <strong>System Information</strong>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <v-list density="compact">
                <v-list-item>
                  <v-list-item-title>SDK Version</v-list-item-title>
                  <v-list-item-subtitle>{{ diagnosticData.system.sdk_version }}</v-list-item-subtitle>
                </v-list-item>
                <v-list-item>
                  <v-list-item-title>Arduino Version</v-list-item-title>
                  <v-list-item-subtitle>{{ diagnosticData.system.arduino_version }}</v-list-item-subtitle>
                </v-list-item>
                <v-list-item>
                  <v-list-item-title>Uptime</v-list-item-title>
                  <v-list-item-subtitle>{{ formatUptime(diagnosticData.system.uptime_ms) }}</v-list-item-subtitle>
                </v-list-item>
                <v-list-item>
                  <v-list-item-title>Reset Reason</v-list-item-title>
                  <v-list-item-subtitle>{{ diagnosticData.system.reset_reason }}</v-list-item-subtitle>
                </v-list-item>
              </v-list>
            </v-expansion-panel-text>
          </v-expansion-panel>
        </v-expansion-panels>

        <!-- Export Button -->
        <v-card-actions class="mt-4">
          <v-btn
            color="primary"
            variant="elevated"
            prepend-icon="mdi-download"
            @click="exportDiagnostic"
          >
            Export as JSON
          </v-btn>
          <v-btn
            color="secondary"
            variant="text"
            prepend-icon="mdi-content-copy"
            @click="copyToClipboard"
          >
            Copy to Clipboard
          </v-btn>
        </v-card-actions>
      </v-card-text>
    </v-card>

    <!-- Upload Dialog -->
    <v-dialog v-model="showUploadDialog" max-width="600">
      <v-card>
        <v-card-title>Upload Diagnostic Data</v-card-title>
        <v-card-text>
          <v-textarea
            v-model="serialOutput"
            label="Paste Serial Monitor Output"
            placeholder="Paste the entire serial output from the diagnostic firmware here..."
            rows="10"
            variant="outlined"
          ></v-textarea>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn color="grey" variant="text" @click="showUploadDialog = false">
            Cancel
          </v-btn>
          <v-btn color="primary" variant="elevated" @click="parseAndUpload">
            Parse & Upload
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue';

// Data
const loading = ref(false);
const diagnosticData = ref(null);
const showUploadDialog = ref(false);
const serialOutput = ref('');

// I2C table headers
const i2cHeaders = [
  { title: 'Address', key: 'address', sortable: true },
  { title: 'Decimal', key: 'decimal', sortable: true },
  { title: 'Likely Device', key: 'likely', sortable: false }
];

// Computed
const heapUsagePercent = computed(() => {
  if (!diagnosticData.value) return 0;
  const used = diagnosticData.value.memory.heap.used_kb;
  const total = diagnosticData.value.memory.heap.total_kb;
  return Math.round((used / total) * 100);
});

// Methods
const loadDiagnostics = async () => {
  loading.value = true;
  try {
    const response = await fetch('/api/diagnostic/list');
    const data = await response.json();
    
    if (data.devices && data.devices.length > 0) {
      // Load the most recent diagnostic
      const latestDevice = data.devices[0];
      const detailResponse = await fetch(`/api/diagnostic/${latestDevice.device_id}`);
      const detailData = await detailResponse.json();
      diagnosticData.value = detailData.data;
    }
  } catch (error) {
    console.error('Failed to load diagnostics:', error);
  } finally {
    loading.value = false;
  }
};

const parseAndUpload = async () => {
  loading.value = true;
  try {
    const response = await fetch('/api/diagnostic/parse', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ serial_output: serialOutput.value })
    });
    
    const data = await response.json();
    
    if (data.success) {
      diagnosticData.value = data.diagnostic_data.diagnostic_tool;
      showUploadDialog.value = false;
      serialOutput.value = '';
    } else {
      alert('Failed to parse diagnostic data: ' + data.error);
    }
  } catch (error) {
    alert('Error: ' + error.message);
  } finally {
    loading.value = false;
  }
};

const exportDiagnostic = () => {
  const dataStr = JSON.stringify(diagnosticData.value, null, 2);
  const dataBlob = new Blob([dataStr], { type: 'application/json' });
  const url = URL.createObjectURL(dataBlob);
  const link = document.createElement('a');
  link.href = url;
  link.download = `esp32-diagnostic-${Date.now()}.json`;
  link.click();
  URL.revokeObjectURL(url);
};

const copyToClipboard = () => {
  const dataStr = JSON.stringify(diagnosticData.value, null, 2);
  navigator.clipboard.writeText(dataStr);
  alert('Diagnostic data copied to clipboard!');
};

const formatUptime = (ms) => {
  const seconds = Math.floor(ms / 1000);
  const minutes = Math.floor(seconds / 60);
  const hours = Math.floor(minutes / 60);
  return `${hours}h ${minutes % 60}m ${seconds % 60}s`;
};

// Lifecycle
onMounted(() => {
  loadDiagnostics();
});
</script>

<style scoped>
.diagnostic-container {
  padding: 16px;
}

.diagnostic-card {
  border-radius: 12px;
}

.diagnostic-header {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white !important;
}

.diagnostic-content {
  padding: 24px;
}
</style>
