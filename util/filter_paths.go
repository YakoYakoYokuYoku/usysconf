// Copyright © 2019-2020 Solus Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package util

import (
	"path/filepath"
)

// FilterPaths will process through globbed paths and remove any paths from the resulting slice if they are present in the exclude slice.
func FilterPaths(include []string, exclude []string) []string {
	paths := make([]string, 0)

	ipaths := make([]string, 0)
	for _, p := range include {
		ps, err := filepath.Glob(p)
		if err != nil {
			continue
		}

		ipaths = append(paths, ps...)
	}

	epaths := make([]string, 0)
	for _, p := range exclude {
		ps, err := filepath.Glob(p)
		if err != nil {
			continue
		}

		epaths = append(epaths, ps...)
	}

it:
	for _, ip := range ipaths {
		for _, ep := range epaths {
			if ip == ep {
				continue it
			}
		}

		paths = append(paths, ip)
	}

	return paths
}
